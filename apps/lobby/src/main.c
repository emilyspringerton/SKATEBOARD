#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

void draw_canyon() {
    // Render Red Rock Floor
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.3f, 0.1f); // Terracotta Red
    glVertex3f(-100, 0, -100); glVertex3f(100, 0, -100);
    glVertex3f(100, 0, 100); glVertex3f(-100, 0, 100);
    glEnd();

    // Render Boulders (Mesas)
    for(int i=-5; i<5; i++) {
        glPushMatrix();
        glTranslatef(i*15.0f, 0, sinf(i)*20.0f);
        glBegin(GL_TRIANGLES);
        glColor3f(0.6f, 0.2f, 0.1f);
        glVertex3f(0, 10, 0); glVertex3f(-5, 0, -5); glVertex3f(5, 0, -5);
        glVertex3f(0, 10, 0); glVertex3f(5, 0, -5); glVertex3f(0, 0, 5);
        glEnd();
        glPopMatrix();
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("SHANKPIT // CANYON", 100, 100, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(win);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(70.0, 1280.0/720.0, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    int running = 1;
    float cam_y = 2.0f;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) if(e.type == SDL_QUIT) running = 0;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // Dark Sky
        glLoadIdentity();
        
        // Simple Orbit Camera
        float t = SDL_GetTicks() * 0.0005f;
        gluLookAt(sinf(t)*50, 20, cosf(t)*50, 0, 0, 0, 0, 1, 0);

        draw_canyon();

        SDL_GL_SwapWindow(win);
        SDL_Delay(16);
    }
    SDL_Quit();
    return 0;
}
