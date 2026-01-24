#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

// --- SHARED LOGIC ---
void parse_str(const char* json, const char* key, char* dest) {
    char search[64]; sprintf(search, "\"%s\":", key);
    char* found = strstr(json, search);
    if(!found) return;
    char* start = strchr(found, ':'); if(!start) return;
    start = strchr(start, '"'); if(!start) return; start++;
    int i=0; while(start[i]!='"' && i<31) { dest[i]=start[i]; i++; } dest[i]=0;
}

long parse_int(const char* json, const char* key) {
    char search[64]; sprintf(search, "\"%s\":", key);
    char* found = strstr(json, search);
    if(!found) return 0;
    return strtol(strchr(found, ':')+1, NULL, 10);
}

void empire_load(const char* filename, EmpireState* state) {
    FILE* f = fopen(filename, "r");
    if(!f) return;
    char buf[8192]; size_t len = fread(buf, 1, 8192, f); buf[len]=0; fclose(f);
    
    parse_str(buf, "empire_name", state->empire_name);
    state->gold = parse_int(buf, "gold");
    state->hourly_rate = parse_int(buf, "hourly_rate");
    
    state->agent_count = 0;
    char* cursor = buf;
    while( (cursor = strstr(cursor, "name")) && state->agent_count < 64 ) {
        char* obj_start = cursor;
        while(*obj_start != '{' && obj_start > buf) obj_start--;
        Agent* a = &state->agents[state->agent_count++];
        parse_str(obj_start, "name", a->name);
        parse_str(obj_start, "task", a->task);
        parse_str(obj_start, "status", a->status);
        a->xp = parse_int(obj_start, "xp");
        a->x = (float)parse_int(obj_start, "x");
        a->z = (float)parse_int(obj_start, "z");
        cursor += 10;
    }
}

void empire_generate_site(EmpireState* state) {
    FILE* f = fopen("output/index.html", "w");
    if(!f) return;
    fprintf(f, "<html><body style='background:#111;color:#0f0;font-family:monospace'><h1>%s</h1>", state->empire_name);
    fprintf(f, "<p>GOLD: %ld | XP/HR: %ld</p></body></html>", state->gold, state->hourly_rate);
    fclose(f);
}

#ifdef _WIN32
// --- VISUALIZER MODE ---
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include "font.h" // FIXED: Local Include

EmpireState state;
float cam_x=0, cam_y=15, cam_z=20;
float cam_yaw=0, cam_pitch=-30;
int console_open = 0;
char cmd_buf[256] = "";
char log_buf[5][128];

void log_msg(const char* msg) {
    for(int i=4; i>0; i--) strcpy(log_buf[i], log_buf[i-1]);
    strncpy(log_buf[0], msg, 127);
}

void process_command() {
    if (strlen(cmd_buf) == 0) return;
    if (strcmp(cmd_buf, "/help") == 0) {
        log_msg("--- COMMANDS ---");
        log_msg("/status  - Show economy");
        log_msg("/reload  - Reload JSON");
        log_msg("/quit    - Exit");
    }
    else if (strcmp(cmd_buf, "/reload") == 0) {
        empire_load("empire.json", &state);
        log_msg("Reloaded empire.json");
    }
    else if (strcmp(cmd_buf, "/status") == 0) {
        char buf[128]; sprintf(buf, "GOLD: %ld | RATE: %ld/hr", state.gold, state.hourly_rate);
        log_msg(buf);
    }
    else if (strcmp(cmd_buf, "/quit") == 0) exit(0);
    else log_msg("Unknown command.");
    cmd_buf[0] = 0;
}

void draw_cube(float x, float y, float z, float r, float g, float b) {
    glPushMatrix(); glTranslatef(x,y,z); glColor3f(r,g,b);
    glBegin(GL_QUADS);
    glVertex3f(-0.5,0.5,0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,0.5);
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,-0.5,-0.5); glVertex3f(-0.5,-0.5,-0.5);
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(-0.5,0.5,0.5);
    glVertex3f(-0.5,-0.5,-0.5); glVertex3f(0.5,-0.5,-0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,0.5);
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(-0.5,0.5,0.5); glVertex3f(-0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,-0.5);
    glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(0.5,-0.5,-0.5);
    glEnd(); glPopMatrix();
}

int main(int argc, char* argv[]) {
    empire_load("empire.json", &state);
    log_msg("Welcome to Empire Command.");
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("EMPIRE COMMAND v2", 100, 100, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext gl = SDL_GL_CreateContext(win);
    
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(60.0f, 1280.0f/720.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW); glEnable(GL_DEPTH_TEST);

    int running = 1;
    SDL_StartTextInput();
    
    while(running) {
        SDL_Event e; 
        while(SDL_PollEvent(&e)) {
            if(e.type==SDL_QUIT) running=0;
            if (console_open) {
                if(e.type == SDL_KEYDOWN) {
                    if(e.key.keysym.sym == SDLK_RETURN) { process_command(); console_open = 0; }
                    if(e.key.keysym.sym == SDLK_ESCAPE) { console_open = 0; }
                    if(e.key.keysym.sym == SDLK_BACKSPACE && strlen(cmd_buf) > 0) cmd_buf[strlen(cmd_buf)-1] = 0;
                }
                if(e.type == SDL_TEXTINPUT) strcat(cmd_buf, e.text.text);
            } else {
                if(e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_SLASH || e.key.keysym.sym == SDLK_RETURN)) { 
                    console_open = 1; if(e.key.keysym.sym == SDLK_SLASH) strcpy(cmd_buf, "/"); else cmd_buf[0] = 0; 
                }
                if (e.type == SDL_MOUSEMOTION && (SDL_GetMouseState(NULL,NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))) {
                    cam_yaw -= e.motion.xrel * 0.2f; cam_pitch -= e.motion.yrel * 0.2f;
                }
            }
        }
        
        if (!console_open) {
            const Uint8* k = SDL_GetKeyboardState(NULL);
            float r = -cam_yaw * 0.01745f;
            float fwd_x = sinf(r), fwd_z = -cosf(r), right_x = cosf(r), right_z = sinf(r);
            if(k[SDL_SCANCODE_W]) { cam_x += fwd_x*0.5f; cam_z += fwd_z*0.5f; }
            if(k[SDL_SCANCODE_S]) { cam_x -= fwd_x*0.5f; cam_z -= fwd_z*0.5f; }
            if(k[SDL_SCANCODE_D]) { cam_x += right_x*0.5f; cam_z += right_z*0.5f; }
            if(k[SDL_SCANCODE_A]) { cam_x -= right_x*0.5f; cam_z -= right_z*0.5f; }
            if(k[SDL_SCANCODE_SPACE]) cam_y += 0.5f;
            if(k[SDL_SCANCODE_LSHIFT]) cam_y -= 0.5f;
        }

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity(); glRotatef(-cam_pitch, 1, 0, 0); glRotatef(-cam_yaw, 0, 1, 0); glTranslatef(-cam_x, -cam_y, -cam_z);
        
        glBegin(GL_LINES); glColor3f(0.2f, 0.2f, 0.2f);
        for(int i=-50; i<=50; i+=5) { glVertex3f(i,0,-50); glVertex3f(i,0,50); glVertex3f(-50,0,i); glVertex3f(50,0,i); } glEnd();
        
        for(int i=0; i<state.agent_count; i++) {
            Agent* a = &state.agents[i];
            float r = (strcmp(a->status, "active")==0) ? 0.0f : 1.0f; 
            float g = (strcmp(a->status, "active")==0) ? 1.0f : 0.0f; 
            draw_cube(a->x, 0.5f, a->z, r, g, 0.0f);
        }
        
        glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D(0, 1280, 720, 0);
        glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); glDisable(GL_DEPTH_TEST);
        
        draw_text(state.empire_name, 20, 20, 30.0f, 1, 1, 1);
        for(int i=0; i<5; i++) draw_text(log_buf[i], 20, 600 - (i*25), 18.0f, 0.7f, 0.7f, 0.7f);
        
        if (console_open) { char buf[300]; sprintf(buf, "> %s_", cmd_buf); draw_text(buf, 20, 680, 20.0f, 0, 1, 0); } 
        else draw_text("[ / ] to open console", 20, 680, 18.0f, 0.5f, 0.5f, 0.5f);

        glEnable(GL_DEPTH_TEST); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW); glPopMatrix();
        SDL_GL_SwapWindow(win); SDL_Delay(16);
    }
    return 0;
}
#else
int main() { EmpireState s; empire_load("empire.json", &s); empire_generate_site(&s); return 0; }
#endif
