#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "protocol.h"
#include "local_game.h"
#include "map.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// FONT SYSTEM (Inline)
void draw_char(char c, float x, float y, float s) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(s, -s, 1.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    c = toupper(c);
    
    if(c=='H'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(1,0); glVertex2f(1,1); glVertex2f(0,0.5); glVertex2f(1,0.5); }
    if(c=='P'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0,0.5); }
    if(c=='A'){ glVertex2f(0,0); glVertex2f(0.5,1); glVertex2f(0.5,1); glVertex2f(1,0); glVertex2f(0.2,0.4); glVertex2f(0.8,0.4); }
    if(c=='M'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0.5,0.5); glVertex2f(0.5,0.5); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); }
    if(c=='O'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); }
    if(c=='W'){ glVertex2f(0,1); glVertex2f(0.2,0); glVertex2f(0.2,0); glVertex2f(0.5,0.5); glVertex2f(0.5,0.5); glVertex2f(0.8,0); glVertex2f(0.8,0); glVertex2f(1,1); }
    if(c=='R'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0); }
    if(c=='T'){ glVertex2f(0.5,0); glVertex2f(0.5,1); glVertex2f(0,1); glVertex2f(1,1); }
    if(c=='G'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0.5,0.5); }
    if(c=='D'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0.8,0.5); glVertex2f(0.8,0.5); glVertex2f(0,0); }
    if(c=='E'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); glVertex2f(0,0.5); glVertex2f(0.8,0.5); }
    if(c=='I'){ glVertex2f(0.5,0); glVertex2f(0.5,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(0,0); glVertex2f(1,0); }
    if(c=='S'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); }
    if(c=='0'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); }
    if(c=='1'){ glVertex2f(0.5,0); glVertex2f(0.5,1); }
    if(c=='2'){ glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); }
    if(c=='3'){ glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); glVertex2f(0,0.5); glVertex2f(1,0.5); }
    if(c=='4'){ glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0.5); glVertex2f(1,1); glVertex2f(1,0); }
    if(c=='5'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); }
    if(c=='6'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0,0.5); }
    if(c=='7'){ glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(0,0); }
    if(c=='8'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); glVertex2f(0,0.5); glVertex2f(1,0.5); }
    if(c=='9'){ glVertex2f(1,0); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0.5); }
    if(c==' '){ /* Space */ }
    
    glEnd();
    glPopMatrix();
}

void draw_text(const char* str, float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    float cx = x;
    while(*str) {
        draw_char(*str, cx, y, size);
        cx += size * 1.5f;
        str++;
    }
}

ServerState local_state;
int local_pid = 0;
extern GameMap local_level;

// RECOIL STATE
float recoil_offset_y = 0.0f;
float recoil_velocity = 0.0f;
float weapon_sway_x = 0.0f;
float weapon_sway_y = 0.0f;

// NEON TRAIL SYSTEM
typedef struct {
    Vec3 pos;
    float life;
    float r, g, b;
} Trail;

#define MAX_TRAILS 200
Trail trails[MAX_TRAILS];
int trail_idx = 0;

void add_trail(Vec3 pos, float r, float g, float b) {
    trails[trail_idx].pos = pos;
    trails[trail_idx].life = 1.0f;
    trails[trail_idx].r = r;
    trails[trail_idx].g = g;
    trails[trail_idx].b = b;
    trail_idx = (trail_idx + 1) % MAX_TRAILS;
}

void update_trails(float dt) {
    for(int i=0; i<MAX_TRAILS; i++) {
        if(trails[i].life > 0) {
            trails[i].life -= dt * 2.0f;
        }
    }
}

void render_trails() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    glBegin(GL_POINTS);
    glPointSize(6.0f);
    for(int i=0; i<MAX_TRAILS; i++) {
        if(trails[i].life > 0) {
            float alpha = trails[i].life;
            glColor4f(trails[i].r, trails[i].g, trails[i].b, alpha);
            glVertex3f(trails[i].pos.x, trails[i].pos.y, trails[i].pos.z);
        }
    }
    glEnd();
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

// WEAPON RECOIL SYSTEM
void apply_recoil(int weapon_id) {
    switch(weapon_id) {
        case WPN_KNIFE:   recoil_velocity = 0.0f; break;
        case WPN_MAGNUM:  recoil_velocity = 8.0f; break;
        case WPN_AR:      recoil_velocity = 4.0f; break;
        case WPN_SHOTGUN: recoil_velocity = 12.0f; break;
        case WPN_SNIPER:  recoil_velocity = 15.0f; break;
    }
}

void update_recoil(float dt) {
    // Spring physics for recoil
    float spring_force = -recoil_offset_y * 0.4f;
    float damping = -recoil_velocity * 0.3f;
    
    recoil_velocity += (spring_force + damping) * dt * 60.0f;
    recoil_offset_y += recoil_velocity * dt * 60.0f;
    
    // Weapon sway
    weapon_sway_x *= 0.95f;
    weapon_sway_y *= 0.95f;
}

void draw_weapon(int weapon_id, float recoil) {
    glPushMatrix();
    
    // Position in view
    glTranslatef(0.4f + weapon_sway_x, -0.3f + recoil + weapon_sway_y, -0.8f);
    glRotatef(weapon_sway_x * 5.0f, 0, 1, 0);
    
    // Weapon-specific models
    switch(weapon_id) {
        case WPN_KNIFE:
            glColor3f(0.8f, 0.8f, 0.9f);
            glScalef(0.05f, 0.05f, 0.3f);
            break;
        case WPN_MAGNUM:
            glColor3f(0.3f, 0.3f, 0.4f);
            glScalef(0.08f, 0.12f, 0.25f);
            break;
        case WPN_AR:
            glColor3f(0.2f, 0.5f, 0.3f);
            glScalef(0.08f, 0.12f, 0.5f);
            break;
        case WPN_SHOTGUN:
            glColor3f(0.5f, 0.3f, 0.2f);
            glScalef(0.12f, 0.15f, 0.45f);
            break;
        case WPN_SNIPER:
            glColor3f(0.15f, 0.15f, 0.2f);
            glScalef(0.06f, 0.1f, 0.9f);
            break;
    }
    
    // Draw box
    glBegin(GL_QUADS);
    glVertex3f(-1,-1,1); glVertex3f(1,-1,1); glVertex3f(1,1,1); glVertex3f(-1,1,1);
    glVertex3f(-1,-1,-1); glVertex3f(-1,1,-1); glVertex3f(1,1,-1); glVertex3f(1,-1,-1);
    glVertex3f(-1,-1,-1); glVertex3f(-1,-1,1); glVertex3f(-1,1,1); glVertex3f(-1,1,-1);
    glVertex3f(1,-1,-1); glVertex3f(1,1,-1); glVertex3f(1,1,1); glVertex3f(1,-1,1);
    glVertex3f(-1,1,-1); glVertex3f(-1,1,1); glVertex3f(1,1,1); glVertex3f(1,1,-1);
    glVertex3f(-1,-1,-1); glVertex3f(1,-1,-1); glVertex3f(1,-1,1); glVertex3f(-1,-1,1);
    glEnd();
    
    glPopMatrix();
}

void draw_vehicle() {
    VehicleState *v = &local_state.vehicle;
    if(!v->active) return;
    
    glPushMatrix();
    glTranslatef(v->pos.x, v->pos.y, v->pos.z);
    glRotatef(v->yaw, 0, 1, 0);
    glRotatef(v->pitch, 1, 0, 0);
    glRotatef(v->roll, 0, 0, 1);
    
    // NEON GLOW EFFECT
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    // Outer glow
    glColor4f(0.0f, 1.0f, 1.0f, 0.3f);
    glScalef(2.2f, 1.2f, 3.2f);
    glBegin(GL_QUADS);
    glVertex3f(-1,-0.5f,1); glVertex3f(1,-0.5f,1); glVertex3f(1,0.5f,1); glVertex3f(-1,0.5f,1);
    glVertex3f(-1,-0.5f,-1); glVertex3f(-1,0.5f,-1); glVertex3f(1,0.5f,-1); glVertex3f(1,-0.5f,-1);
    glEnd();
    
    glLoadIdentity();
    glTranslatef(v->pos.x, v->pos.y, v->pos.z);
    glRotatef(v->yaw, 0, 1, 0);
    glRotatef(v->pitch, 1, 0, 0);
    glRotatef(v->roll, 0, 0, 1);
    
    // Core body
    glDisable(GL_BLEND);
    glColor3f(0.2f, 0.3f, 0.4f);
    glScalef(2.0f, 1.0f, 3.0f);
    glBegin(GL_QUADS);
    glVertex3f(-1,-0.5f,1); glVertex3f(1,-0.5f,1); glVertex3f(1,0.5f,1); glVertex3f(-1,0.5f,1);
    glVertex3f(-1,-0.5f,-1); glVertex3f(-1,0.5f,-1); glVertex3f(1,0.5f,-1); glVertex3f(1,-0.5f,-1);
    glVertex3f(-1,-0.5f,-1); glVertex3f(-1,-0.5f,1); glVertex3f(-1,0.5f,1); glVertex3f(-1,0.5f,-1);
    glVertex3f(1,-0.5f,-1); glVertex3f(1,0.5f,-1); glVertex3f(1,0.5f,1); glVertex3f(1,-0.5f,1);
    glVertex3f(-1,0.5f,-1); glVertex3f(-1,0.5f,1); glVertex3f(1,0.5f,1); glVertex3f(1,0.5f,-1);
    glVertex3f(-1,-0.5f,-1); glVertex3f(1,-0.5f,-1); glVertex3f(1,-0.5f,1); glVertex3f(-1,-0.5f,1);
    glEnd();
    
    glPopMatrix();
}

void draw_map() {
    extern GameMap local_level;
    
    for(int i=0; i<local_level.wall_count; i++) {
        Wall *w = &local_level.walls[i];
        
        glPushMatrix();
        glTranslatef(w->x, w->y, w->z);
        glScalef(w->sx, w->sy, w->sz);
        
        // Base color
        glColor3f(w->r, w->g, w->b);
        
        glBegin(GL_QUADS);
        glVertex3f(-1,-1,1); glVertex3f(1,-1,1); glVertex3f(1,1,1); glVertex3f(-1,1,1);
        glVertex3f(-1,-1,-1); glVertex3f(-1,1,-1); glVertex3f(1,1,-1); glVertex3f(1,-1,-1);
        glVertex3f(-1,-1,-1); glVertex3f(-1,-1,1); glVertex3f(-1,1,1); glVertex3f(-1,1,-1);
        glVertex3f(1,-1,-1); glVertex3f(1,1,-1); glVertex3f(1,1,1); glVertex3f(1,-1,1);
        glVertex3f(-1,1,-1); glVertex3f(-1,1,1); glVertex3f(1,1,1); glVertex3f(1,1,-1);
        glVertex3f(-1,-1,-1); glVertex3f(1,-1,-1); glVertex3f(1,-1,1); glVertex3f(-1,-1,1);
        glEnd();
        
        // NEON EDGES (Slippery surfaces only)
        if(w->friction > 0.95f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(0.0f, 1.0f, 1.0f, 0.5f);
            glLineWidth(2.0f);
            
            glBegin(GL_LINES);
            // Wireframe edges
            glVertex3f(-1,-1,-1); glVertex3f(1,-1,-1);
            glVertex3f(1,-1,-1); glVertex3f(1,1,-1);
            glVertex3f(1,1,-1); glVertex3f(-1,1,-1);
            glVertex3f(-1,1,-1); glVertex3f(-1,-1,-1);
            
            glVertex3f(-1,-1,1); glVertex3f(1,-1,1);
            glVertex3f(1,-1,1); glVertex3f(1,1,1);
            glVertex3f(1,1,1); glVertex3f(-1,1,1);
            glVertex3f(-1,1,1); glVertex3f(-1,-1,1);
            
            glVertex3f(-1,-1,-1); glVertex3f(-1,-1,1);
            glVertex3f(1,-1,-1); glVertex3f(1,-1,1);
            glVertex3f(1,1,-1); glVertex3f(1,1,1);
            glVertex3f(-1,1,-1); glVertex3f(-1,1,1);
            glEnd();
            
            glDisable(GL_BLEND);
        }
        
        glPopMatrix();
    }
}

void draw_hud() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    
    PlayerState *p = &local_state.players[local_pid];
    char buf[128];
    
    // HP
    sprintf(buf, "HP %d", p->health);
    draw_text(buf, 50, WINDOW_HEIGHT - 80, 40.0f, 
              p->health > 50 ? 0.0f : 1.0f, 
              p->health > 50 ? 1.0f : 0.0f, 
              0.2f);
    
    // Ammo
    sprintf(buf, "AMMO %d", p->ammo[p->current_weapon]);
    draw_text(buf, WINDOW_WIDTH - 300, WINDOW_HEIGHT - 80, 40.0f, 0.0f, 1.0f, 1.0f);
    
    // Vehicle status
    if(p->in_vehicle) {
        draw_text("WARTHOG MODE", WINDOW_WIDTH/2 - 150, 100, 30.0f, 0.0f, 1.0f, 1.0f);
    }
    
    // Crosshair (with neon glow)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glLineWidth(3.0f);
    
    // Outer glow
    glBegin(GL_LINES);
    glColor4f(0.0f, 1.0f, 1.0f, 0.5f);
    glVertex2f(WINDOW_WIDTH/2 - 20, WINDOW_HEIGHT/2);
    glVertex2f(WINDOW_WIDTH/2 + 20, WINDOW_HEIGHT/2);
    glVertex2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 20);
    glVertex2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 20);
    glEnd();
    
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex2f(WINDOW_WIDTH/2 - 15, WINDOW_HEIGHT/2);
    glVertex2f(WINDOW_WIDTH/2 + 15, WINDOW_HEIGHT/2);
    glVertex2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 15);
    glVertex2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 15);
    glEnd();
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("SHANKPIT // GOLDILOCKS v123", 
                                       100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 
                                       SDL_WINDOW_OPENGL);
    SDL_GLContext gl = SDL_GL_CreateContext(win);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    glMatrixMode(GL_PROJECTION);
    gluPerspective(70.0, (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    
    local_init();
    
    float dt = 0.016f;
    Uint32 last_time = SDL_GetTicks();
    int running = 1;
    
    int last_shoot = 0;
    
    while(running) {
        Uint32 current_time = SDL_GetTicks();
        dt = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        
        SDL_Event e;
        float mouse_dx = 0, mouse_dy = 0;
        
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = 0;
            if(e.type == SDL_MOUSEMOTION) {
                mouse_dx = e.motion.xrel;
                mouse_dy = e.motion.yrel;
            }
        }
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        Uint32 mouse = SDL_GetMouseState(NULL, NULL);
        
        float fwd = 0, strafe = 0;
        if(keys[SDL_SCANCODE_W]) fwd = 1.0f;
        if(keys[SDL_SCANCODE_S]) fwd = -1.0f;
        if(keys[SDL_SCANCODE_D]) strafe = 1.0f;
        if(keys[SDL_SCANCODE_A]) strafe = -1.0f;
        
        int shoot = (mouse & SDL_BUTTON(SDL_BUTTON_LEFT)) ? 1 : 0;
        int use = keys[SDL_SCANCODE_E] ? 1 : 0;
        int jump = keys[SDL_SCANCODE_SPACE] ? 1 : 0;
        
        int weapon = -1;
        if(keys[SDL_SCANCODE_1]) weapon = WPN_KNIFE;
        if(keys[SDL_SCANCODE_2]) weapon = WPN_MAGNUM;
        if(keys[SDL_SCANCODE_3]) weapon = WPN_AR;
        if(keys[SDL_SCANCODE_4]) weapon = WPN_SHOTGUN;
        if(keys[SDL_SCANCODE_5]) weapon = WPN_SNIPER;
        
        PlayerState *p = &local_state.players[local_pid];
        static float yaw = 0, pitch = 0;
        
        yaw -= mouse_dx * 0.2f;
        pitch -= mouse_dy * 0.2f;
        if(pitch > 89) pitch = 89;
        if(pitch < -89) pitch = -89;
        
        // Weapon sway from mouse movement
        weapon_sway_x += mouse_dx * 0.001f;
        weapon_sway_y += mouse_dy * 0.001f;
        
        // Apply recoil on shoot
        if(shoot && !last_shoot) {
            apply_recoil(p->current_weapon);
        }
        last_shoot = shoot;
        
        local_update(dt, fwd, strafe, yaw, pitch, shoot, weapon, jump, 0, 0, use);
        update_recoil(dt);
        update_trails(dt);
        
        // Add velocity trails
        if(fabs(p->vel.x) > 0.3f || fabs(p->vel.z) > 0.3f) {
            add_trail(p->pos, 0.0f, 1.0f, 1.0f);
        }
        
        // RENDER
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        
        glRotatef(-pitch, 1, 0, 0);
        glRotatef(-yaw, 0, 1, 0);
        glTranslatef(-p->pos.x, -(p->pos.y + 1.6f), -p->pos.z);
        
        draw_map();
        draw_vehicle();
        render_trails();
        
        // Grid
        glBegin(GL_LINES);
        glColor3f(0.15f, 0.15f, 0.2f);
        for(int i=-100; i<=100; i+=5) {
            glVertex3f(i, 0, -100);
            glVertex3f(i, 0, 100);
            glVertex3f(-100, 0, i);
            glVertex3f(100, 0, i);
        }
        glEnd();
        
        // Draw weapon in view
        glLoadIdentity();
        draw_weapon(p->current_weapon, recoil_offset_y * 0.01f);
        
        draw_hud();
        
        SDL_GL_SwapWindow(win);
        SDL_Delay(1);
    }
    
    SDL_Quit();
    return 0;
}
