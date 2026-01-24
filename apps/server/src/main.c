
#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define SLEEP(x) Sleep(x)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define SLEEP(x) usleep((x)*1000)
#endif

#include "../../../packages/protocol/protocol.h"
#include "../../../packages/simulation/game_physics.h"
#include "../../../packages/map/map.c"
#include "../../../packages/simulation/local_game.h"

ServerState local_state;

long long current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int main() {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    int opt = 1; setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind Failed"); return 1;
    }
    
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    printf("🔥 SERVER LISTENING ON NEW PORT %d (OPTIMIZED MTU)\n", PORT);
    local_init();

    struct sockaddr_in clients[MAX_CLIENTS];
    int client_active[MAX_CLIENTS] = {0};
    long long last_seen[MAX_CLIENTS] = {0};

    while(1) {
        char buf[4096]; struct sockaddr_in from; 
        socklen_t fromlen = sizeof(from);
        long long now = current_time_ms();
        
        int len;
        while ((len = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen)) > 0) {
            Packet *pkt = (Packet*)buf;
            
            // Validate minimal size (type int)
            if (len < sizeof(int)) continue;
            
            int pid = -1;
            for(int i=0; i<MAX_CLIENTS; i++) {
                if (client_active[i] && clients[i].sin_addr.s_addr == from.sin_addr.s_addr && clients[i].sin_port == from.sin_port) {
                    pid = i; break;
                }
            }
            
            if (pid == -1) {
                for(int i=0; i<MAX_CLIENTS; i++) {
                    if (i != 1 && !client_active[i] && !local_state.players[i].active) {
                        client_active[i] = 1; clients[i] = from; pid = i;
                        local_state.players[i].active = 1;
                        local_state.players[i].health = 100;
                        local_state.players[i].pos.y = 5.0f;
                        for(int j=0; j<MAX_WEAPONS; j++) local_state.players[i].ammo[j] = WPN_STATS[j].ammo_max;
                        printf("Join: PID %d\n", pid);
                        break;
                    }
                }
            }
            
            if (pid != -1) {
                last_seen[pid] = now;
                if (pkt->type == PACKET_INPUT) {
                    // Extract Input Data safely (packet might be small now)
                    if (len >= sizeof(int) + sizeof(ClientInput)) {
                        ClientInput in;
                        memcpy(&in, pkt->data, sizeof(ClientInput));
                        local_pid = pid;
                        local_update(0.016f, in.fwd, in.strafe, in.yaw, in.pitch, in.shoot, in.weapon_req, in.jump, in.crouch, in.reload);
                    }
                }
            }
        }

        for(int i=0; i<MAX_CLIENTS; i++) {
            if (client_active[i] && (now - last_seen[i] > 5000)) {
                printf("Timeout: PID %d\n", i);
                client_active[i] = 0;
                local_state.players[i].active = 0;
            }
        }
        
        if (!client_active[1]) {
             PlayerState *bot = &local_state.players[1];
             bot->active = 1;
             bot->yaw += 2.0f;
             bot->pos.x = 5.0f + sinf(now * 0.001f) * 3.0f;
             bot->pos.z = 15.0f + cosf(now * 0.001f) * 3.0f;
             if(bot->pos.y < 5.0f) bot->pos.y = 5.0f; 
             if ((now / 500) % 4 == 0) { bot->current_weapon = WPN_AR; bot->is_shooting = 5; } 
             else { bot->is_shooting = 0; }
        }

        for(int i=0; i<MAX_CLIENTS; i++) {
            if (client_active[i]) {
                Packet out; 
                out.type = PACKET_STATE;
                memcpy(out.data, &local_state, sizeof(local_state));
                
                // CRITICAL FIX: Send only necessary bytes
                int packet_size = sizeof(int) + sizeof(ServerState);
                sendto(sock, (char*)&out, packet_size, 0, (struct sockaddr*)&clients[i], sizeof(clients[i]));
            }
        }
        SLEEP(16);
    }
    return 0;
}
