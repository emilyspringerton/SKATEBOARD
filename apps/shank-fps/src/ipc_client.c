#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/uio.h>
#include "../../../packages/common/ipc_protocol.h"

// Stub for OpenSSL HMAC (Assumes linked against -lcrypto)
#include <openssl/hmac.h>
#include <openssl/sha.h>

#define BUILD_SECRET "shankpit-build-secret"
#define TIMEOUT_SEC 2

static int ipc_sock = -1;
static uint64_t packet_seq = 0;
static uint8_t session_key[32];
static int is_authenticated = 0;

void compute_hmac(const void *data, size_t len, const uint8_t *key, size_t key_len, uint8_t *out) {
    unsigned int out_len;
    HMAC(EVP_sha256(), key, key_len, (const unsigned char*)data, len, out, &out_len);
}

int ipc_connect_and_handshake() {
    if (ipc_sock != -1) close(ipc_sock);
    
    ipc_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (ipc_sock == -1) return -1;

    struct timeval tv = {TIMEOUT_SEC, 0};
    setsockopt(ipc_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SHANKPIT_IPC_SOCKET, sizeof(addr.sun_path)-1);

    if (connect(ipc_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) return -1;

    // Send HELLO
    IPCHeader hdr_hello = { MAGIC_HEADER, MSG_HELLO, sizeof(MsgHello) };
    MsgHello hello = { getpid(), 0xCAFEBABE, (uint64_t)time(NULL), (uint64_t)time(NULL) };
    send(ipc_sock, &hdr_hello, sizeof(hdr_hello), 0);
    send(ipc_sock, &hello, sizeof(hello), 0);

    // Recv CHALLENGE
    IPCHeader hdr_resp;
    if (recv(ipc_sock, &hdr_resp, sizeof(hdr_resp), 0) <= 0) return -1;
    MsgChallenge challenge;
    if (recv(ipc_sock, &challenge, sizeof(challenge), 0) <= 0) return -1;

    // Send ATTEST
    MsgAttest attest;
    compute_hmac(challenge.challenge_nonce, 32, (uint8_t*)BUILD_SECRET, strlen(BUILD_SECRET), attest.hmac_signature);
    memcpy(session_key, attest.hmac_signature, 32); 

    IPCHeader hdr_attest = { MAGIC_HEADER, MSG_ATTEST, sizeof(MsgAttest) };
    send(ipc_sock, &hdr_attest, sizeof(hdr_attest), 0);
    send(ipc_sock, &attest, sizeof(attest), 0);

    // Set Non-Blocking
    int flags = fcntl(ipc_sock, F_GETFL, 0);
    fcntl(ipc_sock, F_SETFL, flags | O_NONBLOCK);

    is_authenticated = 1;
    packet_seq = 1;
    printf("[IPC] Proof of Fun Established.\n");
    return 0;
}

void ipc_send_heartbeat(float fps, float entropy) {
    if (ipc_sock == -1 || !is_authenticated) {
        if (rand() % 120 == 0) ipc_connect_and_handshake(); 
        return;
    }

    MsgHeartbeat hb;
    memset(&hb, 0, sizeof(hb));
    hb.seq_id = packet_seq++;
    hb.avg_fps = fps;
    hb.fps_min = fps;
    hb.entropy_score = entropy;
    hb.timestamp = (uint64_t)time(NULL);
    
    compute_hmac(&hb, sizeof(hb) - 32, session_key, 32, hb.hmac);

    IPCHeader hdr = { MAGIC_HEADER, MSG_HEARTBEAT, sizeof(MsgHeartbeat) };
    struct iovec iov[2] = { {&hdr, sizeof(hdr)}, {&hb, sizeof(hb)} };
    
    if (writev(ipc_sock, iov, 2) == -1) {
        if (errno == EPIPE) { close(ipc_sock); ipc_sock = -1; is_authenticated = 0; }
    }
}