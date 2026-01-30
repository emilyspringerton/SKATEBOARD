#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <math.h>
#include "physics.h"

#define SCREEN_W 1280
#define SCREEN_H 720

void draw_grid() {
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.4f, 0.0f);
    for(int i=-50; i<=50; i++) {
        glVertex3f((float)i, 0, -50); glVertex3f((float)i, 0, 50);
        glVertex3f(-50, 0, (float)i); glVertex3f(50, 0, (float)i);
    }
    glEnd();
}

void draw_monolith(float x, float z, float h) {
    glPushMatrix();
    glTranslatef(x, h/2.0f, z);
    glScalef(1.0f, h, 1.0f);
    glBegin(GL_QUADS);
    // Simple Cube face colors for depth
    glColor3f(0.1f, 0.1f, 0.2f); glVertex3f(-1,-1, 1); glVertex3f( 1,-1, 1); glVertex3f( 1, 1, 1); glVertex3f(-1, 1, 1);
    glColor3f(0.05f, 0.05f, 0.1f); glVertex3f(-1,-1,-1); glVertex3f(-1, 1,-1); glVertex3f( 1, 1,-1); glVertex3f( 1,-1,-1);
    glEnd();
    glPopMatrix();
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("SHANKPIT // PARITY_v1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(win);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (double)SCREEN_W/SCREEN_H, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);

    Entity p = { {0, 2, 5}, {0, 0, 0}, 0, 0 };
    int running = 1;
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) { if (e.type == SDL_QUIT) running = 0; }

        // Input Mapping (Classic Shank Controls)
        float fwd = 0, strafe = 0;
        if (keys[SDL_SCANCODE_W]) fwd += 1.0f;
        if (keys[SDL_SCANCODE_S]) fwd -= 1.0f;
        if (keys[SDL_SCANCODE_A]) strafe -= 1.0f;
        if (keys[SDL_SCANCODE_D]) strafe += 1.0f;

        // Apply Physics (The Slippery Logic from Phase 013)
        phys_apply_friction(&p);
        float r = -p.yaw * (M_PI/180.0f);
        float wish_x = (sinf(r) * fwd) + (cosf(r) * strafe);
        float wish_z = (-cosf(r) * fwd) + (sinf(r) * strafe);
        phys_accelerate(&p, wish_x, wish_z, MAX_SPEED, ACCEL);
        
        p.pos.x += p.vel.x; p.pos.z += p.vel.z;

        // Render Frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glRotatef(p.pitch, 1, 0, 0);
        glRotatef(p.yaw, 0, 1, 0);
        glTranslatef(-p.pos.x, -p.pos.y, -p.pos.z);

        draw_grid();
        draw_monolith(5, -10, 8);
        draw_monolith(-8, -15, 12);

        SDL_GL_SwapWindow(win);
        SDL_Delay(16);
    }
    SDL_Quit();
    return 0;
}
