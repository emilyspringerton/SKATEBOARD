#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../../../packages/protocol/protocol.h"
#include "../../../packages/simulation/game_physics.h" 
#include "../../../packages/map/map.h" 
#pragma comment(lib, "ws2_32.lib")
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include "../../shank-fps/src/font.h"
#include "../../shank-fps/src/particles.h" 
#include "../../../packages/simulation/local_game.h" 

#define STATE_LOBBY 0
#define STATE_GAME_NET 1
#define STATE_GAME_LOCAL 2
int app_state = STATE_LOBBY;

SOCKET sock; struct sockaddr_in server_addr; long long last_packet_time = 0;
ServerState local_state; int my_player_id = 0; int local_pid = 0;
char lobby_status[128] = "STATUS: IDLE"; char my_name[32] = "Guest"; 
float cam_yaw = 0; float cam_pitch = 0; int zoom_held = 0; 
long long current_ms() { return (long long)SDL_GetTicks(); }

void net_init() {
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    u_long mode = 1; ioctlsocket(sock, FIONBIO, &mode);
    server_addr.sin_family = AF_INET; server_addr.sin_port = htons(PORT);
    struct hostent *he = gethostbyname("s.farthq.com");
    if (he) server_addr.sin_addr = *((struct in_addr*)he->h_addr); else server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
}
void net_send_cmd(int cmd) {
    Packet pkt; pkt.type = PACKET_LOBBY_CMD; pkt.cmd_id = cmd;
    if (cmd == CMD_NAME) strncpy(pkt.data, my_name, 31);
    sendto(sock, (char*)&pkt, sizeof(int)*3 + (cmd==CMD_NAME?32:0), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}
void net_send_input(float fwd, float strafe, int jump, int crouch, int shoot, int reload, int weapon, int zoom, int use) {
    Packet pkt; pkt.type = PACKET_INPUT; pkt.owner_id = my_player_id;
    ClientInput in = {fwd, strafe, cam_yaw, cam_pitch, jump, crouch, shoot, reload, weapon, zoom, use};
    memcpy(pkt.data, &in, sizeof(ClientInput));
    sendto(sock, (char*)&pkt, sizeof(int)*3 + sizeof(ClientInput), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}
void net_update() {
    char buf[4096]; int len;
    while((len = recv(sock, buf, sizeof(buf), 0)) > 0) {
        Packet *pkt = (Packet*)buf;
        if (pkt->type == PACKET_STATE && len >= sizeof(int)*3 + sizeof(ServerState)) {
            memcpy(&local_state, pkt->data, sizeof(ServerState)); my_player_id = pkt->owner_id; local_pid = my_player_id; last_packet_time = current_ms();
        } else if (pkt->type == PACKET_LOBBY_INFO) {
             ServerState info; memcpy(&info, pkt->data, sizeof(ServerState)); sprintf(lobby_status, "%s", info.status_msg);
        }
    }
}
void draw_cube(float sx, float sy, float sz) {
    glPushMatrix(); glScalef(sx, sy, sz); glBegin(GL_QUADS);
    glVertex3f(-0.5,-0.5,0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(-0.5,0.5,0.5);
    glVertex3f(-0.5,-0.5,-0.5); glVertex3f(-0.5,0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,-0.5,-0.5);
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(-0.5,0.5,0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,0.5,-0.5);
    glVertex3f(-0.5,-0.5,-0.5); glVertex3f(0.5,-0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(-0.5,0.5,-0.5);
    glVertex3f(-0.5,-0.5,-0.5); glVertex3f(-0.5,-0.5,0.5); glVertex3f(-0.5,0.5,0.5); glVertex3f(-0.5,0.5,-0.5);
    glVertex3f(-0.5,-0.5,0.5); glVertex3f(-0.5,0.5,0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,-0.5,0.5);
    glEnd(); glPopMatrix();
}
void draw_vehicle(VehicleState *v) {
    glPushMatrix(); glTranslatef(v->pos.x, v->pos.y, v->pos.z); glRotatef(-v->yaw, 0, 1, 0);
    glRotatef(v->pitch*20.0f, 1, 0, 0); glRotatef(v->roll*20.0f, 0, 0, 1);
    glColor3f(0.3f, 0.35f, 0.3f); glPushMatrix(); glTranslatef(0, 0.8f, 0); draw_cube(2.2f, 0.8f, 4.0f); glPopMatrix();
    glColor3f(0.1f, 0.1f, 0.1f); float w=1.4f, l=1.8f, h=0.4f;
    glPushMatrix(); glTranslatef(w, h, l); draw_cube(0.5f, 0.8f, 0.8f); glPopMatrix();
    glPushMatrix(); glTranslatef(-w, h, l); draw_cube(0.5f, 0.8f, 0.8f); glPopMatrix();
    glPushMatrix(); glTranslatef(w, h, -l); draw_cube(0.5f, 0.8f, 0.8f); glPopMatrix();
    glPushMatrix(); glTranslatef(-w, h, -l); draw_cube(0.5f, 0.8f, 0.8f); glPopMatrix();
    glPopMatrix();
}
void render_game() {
    glClearColor(0.4f, 0.6f, 0.9f, 1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    PlayerState *me = &local_state.players[my_player_id];
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(70.0f, 1280.0f/720.0f, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    if (me->in_vehicle) {
        float r = -cam_yaw * 0.01745f, dist = 10.0f;
        gluLookAt(me->pos.x - sinf(r)*dist, me->pos.y + 5.0f, me->pos.z + cosf(r)*dist, me->pos.x, me->pos.y + 1.0f, me->pos.z, 0, 1, 0);
    } else {
        glRotatef(-cam_pitch, 1.0f, 0.0f, 0.0f); glRotatef(-cam_yaw, 0.0f, 1.0f, 0.0f); glTranslatef(-me->pos.x, -(me->pos.y + 1.7f), -me->pos.z);
    }
    
    for(int i=0; i<local_level.wall_count; i++) {
        Wall *w = &local_level.walls[i];
        glPushMatrix(); glTranslatef(w->x, w->y, w->z); glScalef(w->sx, w->sy, w->sz);
        if (w->id == 1) glColor3f(0.7f, 0.5f, 0.3f);      
        else if (w->id == 2) glColor3f(0.6f, 0.3f, 0.2f); 
        else if (w->id == 3) glColor3f(0.5f, 0.5f, 0.6f); 
        else glColor3f(0.4f, 0.3f, 0.2f);                 
        draw_cube(1,1,1); glPopMatrix();
    }
    if (local_state.vehicle.active) draw_vehicle(&local_state.vehicle);
    glEnable(GL_DEPTH_TEST); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW); glPopMatrix();
}
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO); SDL_Window *win = SDL_CreateWindow("SHANK PIT v45 RED ROCK", 100, 100, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext gl = SDL_GL_CreateContext(win);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, 1280, 720, 0); glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    net_init(); SDL_StartTextInput(); particles_init(); last_packet_time = current_ms(); int running = 1; local_init(); 
    while(running) {
        if (app_state != STATE_GAME_LOCAL) net_update();
        SDL_Event e; while(SDL_PollEvent(&e)) { if(e.type == SDL_QUIT) running = 0; if (e.type == SDL_KEYDOWN) { if (e.key.keysym.sym == SDLK_j) app_state = STATE_GAME_NET; } if (e.type == SDL_MOUSEMOTION) cam_yaw -= e.motion.xrel * 0.1f; }
        if (app_state != STATE_LOBBY) {
            const Uint8 *k = SDL_GetKeyboardState(NULL); float fwd=0, str=0; if(k[SDL_SCANCODE_W]) fwd++; if(k[SDL_SCANCODE_S]) fwd--; if(k[SDL_SCANCODE_D]) str++; if(k[SDL_SCANCODE_A]) str--;
            if(app_state==STATE_GAME_NET) net_send_input(fwd, str, k[SDL_SCANCODE_SPACE], k[SDL_SCANCODE_LCTRL], 0, k[SDL_SCANCODE_R], 0, 0, k[SDL_SCANCODE_E]);
            else { local_pid = 0; local_update(0.016f, fwd, str, cam_yaw, cam_pitch, 0, 0, k[SDL_SCANCODE_SPACE], k[SDL_SCANCODE_LCTRL], k[SDL_SCANCODE_R], k[SDL_SCANCODE_E]); }
        }
        render_game(); SDL_GL_SwapWindow(win); SDL_Delay(16);
    }
    return 0;
}
