
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

ServerState state;
GameMap map;
int game_timer = 0;

// TIMEOUT TRACKING
long long last_seen[MAX_CLIENTS];

long long current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void reset_game() {
    printf("RESETTING GAME...\n");
    game_timer = GAME_DURATION;
    phys_init_map(&map);
    memset(&state, 0, sizeof(state));
    
    for(int i=0; i<MAX_CLIENTS; i++) {
        state.players[i].active = 0;
        state.players[i].health = 100;
        state.players[i].pos.y = 5.0f;
        state.players[i].current_weapon = WPN_MAGNUM;
        for(int j=0; j<MAX_WEAPONS; j++) state.players[i].ammo[j] = WPN_STATS[j].ammo_max;
        last_seen[i] = 0;
    }
    
    // Server Bot (Always Active)
    state.players[1].active = 1;
    state.players[1].pos.z = 10.0f;
    last_seen[1] = current_time_ms() + 999999999; // Never timeout
}

int main() {
    #ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
    #endif
    
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) { perror("Socket Failed"); return 1; }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind Failed"); return 1;
    }
    
    #ifdef _WIN32
        u_long mode = 1; ioctlsocket(sock, FIONBIO, &mode);
    #else
        int flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    #endif

    struct sockaddr_in clients[MAX_CLIENTS];
    int client_active[MAX_CLIENTS] = {0};
    
    printf("SHANK SERVER LISTENING ON PORT %d\n", PORT);
    reset_game();

    while(1) {
        char buf[1024]; struct sockaddr_in from; 
        socklen_t fromlen = sizeof(from);
        long long now = current_time_ms();
        
        // 1. RECEIVE
        while (recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen) > 0) {
            Packet *pkt = (Packet*)buf;
            
            int pid = -1;
            // Match existing client
            for(int i=0; i<MAX_CLIENTS; i++) {
                if (client_active[i] && clients[i].sin_addr.s_addr == from.sin_addr.s_addr && clients[i].sin_port == from.sin_port) {
                    pid = i; break;
                }
            }
            // New Client?
            if (pid == -1) {
                for(int i=0; i<MAX_CLIENTS; i++) {
                    // Only take slot if inactive
                    if (!state.players[i].active && i != 1) { 
                        client_active[i] = 1; clients[i] = from; pid = i;
                        state.players[i].active = 1;
                        state.players[i].health = 100; // Reset HP on join
                        printf("Client Joined: PID %d\n", pid);
                        break;
                    }
                }
            }

            if (pid != -1) {
                last_seen[pid] = now; // Update heartbeat
                
                if (pkt->type == PACKET_INPUT) {
                    ClientInput *in = (ClientInput*)pkt->data;
                    PlayerState *p = &state.players[pid];
                    
                    phys_update_player(&map, p, in->fwd, in->strafe, in->yaw, in->pitch, in->jump, in->crouch);
                    
                    if (in->reload) { 
                        if (p->reload_timer <= 0 && p->ammo[p->current_weapon] < WPN_STATS[p->current_weapon].ammo_max) 
                            p->reload_timer = RELOAD_TIME;
                    }
                    if (in->shoot) {
                        if (p->attack_cooldown <= 0 && p->reload_timer <= 0 && p->ammo[p->current_weapon] > 0) {
                            p->ammo[p->current_weapon]--;
                            p->attack_cooldown = WPN_STATS[p->current_weapon].rof;
                            p->is_shooting = 5;
                            
                            Vec3 origin = { p->pos.x, p->pos.y + 1.5f, p->pos.z };
                            Vec3 dir = phys_get_aim(p->yaw, p->pitch, 0);
                            for(int i=0; i<MAX_CLIENTS; i++) {
                                if (i == pid || !state.players[i].active) continue;
                                if (phys_ray_hit(origin, dir, state.players[i].pos, 1.5f)) {
                                    p->hit_feedback = 2; 
                                    state.players[i].health -= WPN_STATS[p->current_weapon].dmg;
                                    if (state.players[i].health <= 0) {
                                        p->kills++;
                                        state.players[i].health = 100;
                                        state.players[i].pos.x = (rand()%20)-10;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // 2. TIMEOUT CHECK (Zombie Killer)
        for (int i=0; i<MAX_CLIENTS; i++) {
            if (i != 1 && state.players[i].active) { // Don't kick bot
                if (now - last_seen[i] > 5000) { // 5 Seconds
                    printf("Client PID %d Timed Out (ZOMBIE KILL)\n", i);
                    state.players[i].active = 0;
                    client_active[i] = 0;
                }
            }
        }

        // 3. GAME LOOP
        state.server_tick++;
        game_timer--;
        if (game_timer <= 0) reset_game();

        if (state.players[1].active) {
            state.players[1].yaw += 2.0f;
            phys_update_player(&map, &state.players[1], 0, 0, state.players[1].yaw, 0, 0, 0);
        }

        // 4. BROADCAST
        for(int i=0; i<MAX_CLIENTS; i++) {
            if (client_active[i]) {
                Packet out; out.type = PACKET_STATE;
                memcpy(out.data, &state, sizeof(state));
                sendto(sock, (char*)&out, sizeof(out), 0, (struct sockaddr*)&clients[i], sizeof(clients[i]));
            }
        }

        SLEEP(16);
    }
    return 0;
}
