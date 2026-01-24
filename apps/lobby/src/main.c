#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../../../packages/protocol/protocol.h"
#include "../../../packages/simulation/game_physics.h" 
#include "../../../packages/map/map.c"
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

SOCKET sock;
struct sockaddr_in server_addr;
long long last_packet_time = 0;
ServerState local_state; 
int my_player_id = 0;
char lobby_status[128] = "IDLE";

float cam_yaw = 0; float cam_pitch = 0;
int zoom_held = 0; 

long long current_ms() { return (long long)SDL_GetTicks(); }

void net_init() {
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    u_long mode = 1; ioctlsocket(sock, FIONBIO, &mode);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    struct hostent *he = gethostbyname("s.farthq.com");
    if (he) { 
        server_addr.sin_addr = *((struct in_addr*)he->h_addr);
    } else {
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
}

void net_send_cmd(int cmd) {
    Packet pkt;
    pkt.type = PACKET_LOBBY_CMD;
    pkt.cmd_id = cmd;
    sendto(sock, (char*)&pkt, sizeof(int)*3, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void net_send_input(float fwd, float strafe, int jump, int crouch, int shoot, int reload, int weapon, int zoom) {
    Packet pkt; 
    pkt.type = PACKET_INPUT;
    pkt.owner_id = my_player_id;
    ClientInput in;
    in.fwd = fwd; in.strafe = strafe; in.yaw = cam_yaw; in.pitch = cam_pitch;
    in.jump = jump; in.crouch = crouch; in.shoot = shoot; in.reload = reload;
    in.weapon_req = weapon; in.zoom = zoom;
    memcpy(pkt.data, &in, sizeof(ClientInput));
    sendto(sock, (char*)&pkt, sizeof(int)*3 + sizeof(ClientInput), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void net_update() {
    char buf[4096];
    int len;
    while((len = recv(sock, buf, sizeof(buf), 0)) > 0) {
        Packet *pkt = (Packet*)buf;
        if (pkt->type == PACKET_STATE) {
            if (len >= sizeof(int)*3 + sizeof(ServerState)) {
                memcpy(&local_state, pkt->data, sizeof(ServerState));
                my_player_id = pkt->owner_id;
                last_packet_time = current_ms();
            }
        }
        else if (pkt->type == PACKET_LOBBY_INFO) {
             ServerState info;
             memcpy(&info, pkt->data, sizeof(ServerState));
             sprintf(lobby_status, "%s", info.status_msg);
        }
    }
}

void draw_cube(float sx, float sy, float sz) {
    glPushMatrix(); glScalef(sx, sy, sz);
    glBegin(GL_QUADS);
    glVertex3f(-0.5,-0.5,0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(-0.5,0.5,0.5);
    glVertex3f(-0.5,-0.5,-0.5); glVertex3f(-0.5,0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,-0.5,-0.5);
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(-0.5,0.5,0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,0.5,-0.5);
    glVertex3f(-0.5,-0.5,-0.5); glVertex3f(0.5,-0.5,-0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,0.5);
    glVertex3f(0.5,-0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,-0.5,0.5);
    glVertex3f(-0.5,-0.5,-0.5); glVertex3f(-0.5,-0.5,0.5); glVertex3f(-0.5,0.5,0.5); glVertex3f(-0.5,0.5,-0.5);
    glEnd(); glPopMatrix();
}

void draw_bot(Vec3 pos, float yaw, int tick) {
    glPushMatrix(); glTranslatef(pos.x, pos.y, pos.z); glRotatef(yaw, 0, 1, 0);
    float breathe = sinf(tick * 0.1f) * 0.05f;
    glColor3f(1.0f, 0.5f, 0.0f); glPushMatrix(); glTranslatef(0, 1.0f + breathe, 0); draw_cube(0.5f, 0.6f, 0.3f); 
    glColor3f(0.3f, 0.3f, 0.3f); glPushMatrix(); glTranslatef(0, 0.5f, 0); draw_cube(0.3f, 0.3f, 0.3f);
    glTranslatef(0, 0.05f, 0.16f); glColor3f(0.0f, 1.0f, 1.0f); draw_cube(0.25f, 0.08f, 0.05f); glPopMatrix(); glPopMatrix();
    glColor3f(0.8f, 0.4f, 0.0f); glPushMatrix(); glTranslatef(-0.4f, 0.2f, 0.0f); glRotatef(15.0f, 0, 0, 1); draw_cube(0.15f, 0.5f, 0.15f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.4f, 0.2f, 0.0f); glRotatef(-15.0f, 0, 0, 1); draw_cube(0.15f, 0.5f, 0.15f); glPopMatrix(); glPopMatrix();
    glColor3f(0.2f, 0.2f, 0.2f); glPushMatrix(); glTranslatef(-0.15f, 0.3f, 0); draw_cube(0.18f, 0.6f, 0.2f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.15f, 0.3f, 0); draw_cube(0.18f, 0.6f, 0.2f); glPopMatrix(); glPopMatrix();
}

void render_gun(PlayerState *me) {
    glLoadIdentity(); glClear(GL_DEPTH_BUFFER_BIT);
    if (me->current_weapon == WPN_SNIPER && zoom_held) return;
    float bob = sinf(local_state.server_tick * 0.2f) * 0.02f * (sqrtf(me->vel.x*me->vel.x + me->vel.z*me->vel.z) * 5.0f);
    float recoil = (me->is_shooting > 0) ? 0.15f : 0.0f;
    glTranslatef(0.3f, -0.3f + bob, -0.5f + recoil); glRotatef(180.0f, 0, 1, 0);
    
    if (me->current_weapon == WPN_KNIFE) { glColor3f(0.6f, 0.6f, 0.6f); glScalef(0.3f, 0.3f, 1.0f); }
    else if (me->current_weapon == WPN_MAGNUM) { glColor3f(0.7f, 0.7f, 0.7f); glScalef(0.5f, 0.5f, 1.0f); }
    else if (me->current_weapon == WPN_AR) { glColor3f(0.1f, 0.1f, 0.1f); glScalef(0.4f, 0.4f, 2.0f); }
    else if (me->current_weapon == WPN_SHOTGUN) { glColor3f(0.4f, 0.2f, 0.1f); glScalef(0.6f, 0.4f, 1.5f); }
    else if (me->current_weapon == WPN_SNIPER) { glColor3f(0.2f, 0.3f, 0.2f); glScalef(0.3f, 0.3f, 2.5f); }

    glBegin(GL_QUADS);
    glVertex3f(-0.05f, 0.05f, 0.3f); glVertex3f(0.05f, 0.05f, 0.3f); glVertex3f(0.05f, -0.05f, 0.3f); glVertex3f(-0.05f, -0.05f, 0.3f);
    glVertex3f(-0.05f, 0.05f, -0.2f); glVertex3f(0.05f, 0.05f, -0.2f); glVertex3f(0.05f, -0.05f, -0.2f); glVertex3f(-0.05f, -0.05f, -0.2f);
    glVertex3f(-0.05f, 0.05f, -0.2f); glVertex3f(0.05f, 0.05f, -0.2f); glVertex3f(0.05f, 0.05f, 0.3f); glVertex3f(-0.05f, 0.05f, 0.3f);
    glVertex3f(0.05f, 0.05f, -0.2f); glVertex3f(0.05f, -0.05f, -0.2f); glVertex3f(0.05f, -0.05f, 0.3f); glVertex3f(0.05f, 0.05f, 0.3f);
    glEnd();
}

void render_game() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    PlayerState *me = &local_state.players[my_player_id];
    float fov = (me->current_weapon == WPN_SNIPER && zoom_held) ? 20.0f : 70.0f;
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(fov, 1280.0f/720.0f, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glRotatef(-cam_pitch, 1.0f, 0.0f, 0.0f); glRotatef(-cam_yaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(-me->pos.x, -(me->pos.y + 1.7f), -me->pos.z);
    
    glBegin(GL_LINES); glColor3f(0.3f, 0.3f, 0.3f); for(int i=-50; i<=50; i+=2) { glVertex3f(i,0,-50); glVertex3f(i,0,50); glVertex3f(-50,0,i); glVertex3f(50,0,i); } glEnd();
    
    if (app_state == STATE_GAME_LOCAL) {
        for(int i=0; i<local_level.wall_count; i++) {
            Wall *w = &local_level.walls[i]; glPushMatrix(); glTranslatef(w->x, w->y, w->z); glScalef(w->sx, w->sy, w->sz);
            glColor3f(w->r, w->g, w->b);
            glBegin(GL_QUADS);
                glVertex3f(-1,-1,1); glVertex3f(1,-1,1); glVertex3f(1,1,1); glVertex3f(-1,1,1);
                glVertex3f(-1,-1,-1); glVertex3f(-1,1,-1); glVertex3f(1,1,-1); glVertex3f(1,-1,-1);
                glVertex3f(-1,-1,-1); glVertex3f(-1,-1,1); glVertex3f(-1,1,1); glVertex3f(-1,1,-1);
                glVertex3f(1,-1,-1); glVertex3f(1,1,-1); glVertex3f(1,1,1); glVertex3f(1,-1,1);
                glVertex3f(-1,1,-1); glVertex3f(-1,1,1); glVertex3f(1,1,1); glVertex3f(1,1,-1);
                glVertex3f(-1,-1,-1); glVertex3f(1,-1,-1); glVertex3f(1,-1,1); glVertex3f(-1,-1,1);
            glEnd(); glPopMatrix();
        }
    }

    for(int i=0; i<MAX_CLIENTS; i++) {
        if (!local_state.players[i].active) continue;
        if (i == my_player_id) continue;
        draw_bot(local_state.players[i].pos, -local_state.players[i].yaw, local_state.server_tick);
        if (me->hit_feedback == 2 && i==1) spawn_particle(local_state.players[i].pos, 5, 0, 0);
    }
    update_render_particles(0.016f); render_gun(me);
    
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D(0, 1280, 720, 0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); glDisable(GL_DEPTH_TEST);
    
    char buf[128]; 
    if (app_state == STATE_LOBBY) {
        // Handled in main loop
    } else {
        sprintf(buf, "HP %d  AMMO %d", me->health, me->ammo[me->current_weapon]); 
        draw_text(buf, 50, 650, 40.0f, 1, 1, 1);
        if (app_state == STATE_GAME_NET) {
            long long timeout = current_ms() - last_packet_time;
            if (timeout > 3000) draw_text("CONNECTION LOST", 480, 360, 30.0f, 1, 0, 0);
        } else {
            draw_text("LOCAL DEMO", 1000, 50, 20.0f, 0, 1, 1);
        }
    }

    if (me->current_weapon == WPN_SNIPER && zoom_held) {
        glColor3f(0, 0, 0); glLineWidth(2.0f); glBegin(GL_LINES);
        glVertex2f(0, 360); glVertex2f(1280, 360); glVertex2f(640, 0); glVertex2f(640, 720); glEnd();
    } else {
        glBegin(GL_LINES); glColor3f(0,1,1); glVertex2f(640-10, 360); glVertex2f(640+10, 360); glVertex2f(640, 360-10); glVertex2f(640, 360+10); glEnd();
    }
    glEnable(GL_DEPTH_TEST); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW); glPopMatrix();
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("SHANK PIT ONLINE v35 LOBBY", 100, 100, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext gl = SDL_GL_CreateContext(win);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, 1280, 720, 0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    net_init(); SDL_StartTextInput(); particles_init(); last_packet_time = current_ms();
    int running = 1;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
            if (app_state == STATE_LOBBY) {
                net_update(); 
                if(e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_c) net_send_cmd(CMD_CREATE);
                    if (e.key.keysym.sym == SDLK_s) net_send_cmd(CMD_SEARCH);
                    if (e.key.keysym.sym == SDLK_j) {
                         SDL_SetRelativeMouseMode(SDL_TRUE); glEnable(GL_DEPTH_TEST);
                         glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(70.0f, 1280.0f/720.0f, 0.1f, 1000.0f); glMatrixMode(GL_MODELVIEW);
                         app_state = STATE_GAME_NET; 
                    }
                    if (e.key.keysym.sym == SDLK_d) {
                         local_init();
                         app_state = STATE_GAME_LOCAL;
                         SDL_SetRelativeMouseMode(SDL_TRUE); glEnable(GL_DEPTH_TEST);
                         glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(70.0f, 1280.0f/720.0f, 0.1f, 1000.0f); glMatrixMode(GL_MODELVIEW);
                    }
                }
            } else {
                if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                    app_state = STATE_LOBBY; SDL_SetRelativeMouseMode(SDL_FALSE);
                    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, 1280, 720, 0); glMatrixMode(GL_MODELVIEW); glLoadIdentity(); glDisable(GL_DEPTH_TEST);
                }
                if(e.type == SDL_MOUSEMOTION) {
                    cam_yaw -= e.motion.xrel * 0.1f;
                    cam_pitch -= e.motion.yrel * 0.1f;
                    if(cam_pitch > 89.0f) cam_pitch = 89.0f; if(cam_pitch < -89.0f) cam_pitch = -89.0f;
                }
            }
        }
        if (app_state == STATE_LOBBY) {
            glClearColor(0.1, 0.1, 0.2, 1); glClear(GL_COLOR_BUFFER_BIT);
            draw_text("SHANK PIT LOBBY", 50, 50, 40.0f, 1, 1, 0);
            draw_text("[C] CREATE MATCH (Reset)", 50, 130, 20.0f, 0, 1, 0);
            draw_text("[S] SEARCH SERVER", 50, 170, 20.0f, 0, 1, 1);
            draw_text("[J] JOIN GAME", 50, 210, 20.0f, 1, 1, 0);
            draw_text("[D] LOCAL DEMO", 50, 250, 20.0f, 1, 1, 1);
            draw_text(lobby_status, 50, 400, 30.0f, 1, 0.5, 0);
        } else {
            const Uint8 *k = SDL_GetKeyboardState(NULL);
            float fwd=0, str=0;
            if(k[SDL_SCANCODE_W]) fwd+=1; if(k[SDL_SCANCODE_S]) fwd-=1;
            if(k[SDL_SCANCODE_D]) str+=1; if(k[SDL_SCANCODE_A]) str-=1;
            int shoot = (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(SDL_BUTTON_LEFT));
            int zoom = (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(SDL_BUTTON_RIGHT));
            zoom_held = zoom;
            int jump = k[SDL_SCANCODE_SPACE];
            int crouch = k[SDL_SCANCODE_LCTRL];
            int reload = k[SDL_SCANCODE_R];
            int weapon = -1;
            if(k[SDL_SCANCODE_1]) weapon = WPN_KNIFE;
            if(k[SDL_SCANCODE_2]) weapon = WPN_MAGNUM;
            if(k[SDL_SCANCODE_3]) weapon = WPN_AR;
            if(k[SDL_SCANCODE_4]) weapon = WPN_SHOTGUN;
            if(k[SDL_SCANCODE_5]) weapon = WPN_SNIPER;

            if (app_state == STATE_GAME_NET) {
                net_update();
                net_send_input(fwd, str, jump, crouch, shoot, reload, weapon, zoom);
            } else {
                local_update(0.016f, fwd, str, cam_yaw, cam_pitch, shoot, weapon, jump, crouch, reload);
            }
            render_game();
        }
        SDL_GL_SwapWindow(win); SDL_Delay(16);
    }
    return 0;
}
