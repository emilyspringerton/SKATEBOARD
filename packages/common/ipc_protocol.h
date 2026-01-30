#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H

#include <stdint.h>

#define SHANKPIT_IPC_SOCKET "/tmp/shankpit_miner.sock"
#define MAGIC_HEADER 0x534B4154 // "SKAT"

enum MsgType {
    MSG_HELLO       = 0x01,
    MSG_CHALLENGE   = 0x02,
    MSG_ATTEST      = 0x03,
    MSG_HEARTBEAT   = 0x04,
    MSG_MINING_CTRL = 0x05,
    MSG_GAME_EVENT  = 0x06
};

typedef struct {
    uint32_t magic;
    uint8_t  type;
    uint32_t payload_len;
} __attribute__((packed)) IPCHeader;

typedef struct {
    uint32_t game_pid;
    uint32_t build_hash;
    uint64_t session_nonce;
    uint64_t timestamp;
} __attribute__((packed)) MsgHello;

typedef struct {
    uint8_t  challenge_nonce[32];
} __attribute__((packed)) MsgChallenge;

typedef struct {
    uint8_t  hmac_signature[32];
} __attribute__((packed)) MsgAttest;

typedef struct {
    uint64_t seq_id;
    float    avg_fps;
    float    fps_min;
    float    entropy_score;
    uint64_t entropy_hash;
    uint64_t timestamp;
    uint8_t  hmac[32];
} __attribute__((packed)) MsgHeartbeat;

#endif