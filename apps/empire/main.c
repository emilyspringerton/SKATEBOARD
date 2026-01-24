#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

// --- MANUAL JSON PARSER ---
// Robust enough for the flat structure we defined
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
    if(!f) { printf("❌ Failed to open %s\n", filename); return; }
    char buf[8192]; size_t len = fread(buf, 1, 8192, f); buf[len]=0; fclose(f);
    
    // Parse Meta
    parse_str(buf, "empire_name", state->empire_name);
    state->gold = parse_int(buf, "gold");
    state->hourly_rate = parse_int(buf, "hourly_rate");
    
    // Parse Agents (Simple scan)
    state->agent_count = 0;
    char* cursor = buf;
    while( (cursor = strstr(cursor, "name")) && state->agent_count < 64 ) {
        // Backup to find the object start '{'
        char* obj_start = cursor;
        while(*obj_start != '{' && obj_start > buf) obj_start--;
        
        Agent* a = &state->agents[state->agent_count++];
        parse_str(obj_start, "name", a->name);
        parse_str(obj_start, "task", a->task);
        parse_str(obj_start, "status", a->status);
        a->xp = parse_int(obj_start, "xp");
        a->x = (float)parse_int(obj_start, "x");
        a->z = (float)parse_int(obj_start, "z");
        
        cursor += 10; // Advance
    }
}

void empire_generate_site(EmpireState* state) {
    FILE* f = fopen("output/index.html", "w");
    if(!f) return;
    
    fprintf(f, "<html><head><title>Empire Command</title>");
    fprintf(f, "<style>body{background:#111;color:#eee;font-family:monospace;text-align:center;}");
    fprintf(f, ".stat{border:1px solid #444;display:inline-block;padding:20px;margin:10px;width:200px;}");
    fprintf(f, ".agent{background:#222;padding:10px;margin:5px auto;max-width:600px;text-align:left;border-left:5px solid #0f0;}");
    fprintf(f, ".active{border-color:#0f0;} .idle{border-color:#aa0;}");
    fprintf(f, "</style></head><body>");
    
    fprintf(f, "<h1>%s DASHBOARD</h1>", state->empire_name);
    fprintf(f, "<div class='stat'><h2>GOLD</h2><p>%ld</p></div>", state->gold);
    fprintf(f, "<div class='stat'><h2>XP/HR</h2><p>%ld</p></div>", state->hourly_rate);
    fprintf(f, "<div class='stat'><h2>AGENTS</h2><p>%d</p></div>", state->agent_count);
    
    fprintf(f, "<h3>LIVE AGENTS</h3>");
    for(int i=0; i<state->agent_count; i++) {
        Agent* a = &state->agents[i];
        fprintf(f, "<div class='agent %s'><strong>%s</strong> [%s]<br>Task: %s | XP: %d | Pos: %.0f, %.0f</div>", 
                a->status, a->name, a->status, a->task, a->xp, a->x, a->z);
    }
    
    fprintf(f, "</body></html>");
    fclose(f);
}

#ifdef _WIN32
// --- VISUALIZER MODE (Windows/SDL) ---
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

void draw_cube(float x, float y, float z, float r, float g, float b) {
    glPushMatrix(); glTranslatef(x,y,z); 
    glColor3f(r,g,b);
    glBegin(GL_QUADS);
    glVertex3f(-0.5,0.5,0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,0.5); // Front
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,-0.5,-0.5); glVertex3f(-0.5,-0.5,-0.5); // Back
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(-0.5,0.5,0.5); // Top
    glVertex3f(-0.5,-0.5,-0.5); glVertex3f(0.5,-0.5,-0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,0.5); // Bottom
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(-0.5,0.5,0.5); glVertex3f(-0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,-0.5); // Left
    glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(0.5,-0.5,-0.5); // Right
    glEnd(); glPopMatrix();
}

int main(int argc, char* argv[]) {
    EmpireState state;
    empire_load("empire.json", &state);
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("EMPIRE VIEWER v1", 100, 100, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext gl = SDL_GL_CreateContext(win);
    
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(60.0f, 1280.0f/720.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    int running = 1;
    float cam_angle = 0;
    
    while(running) {
        SDL_Event e; while(SDL_PollEvent(&e)) if(e.type==SDL_QUIT) running=0;
        
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(0, 20, 30, 0, 0, 0, 0, 1, 0); // God View Camera
        glRotatef(cam_angle, 0, 1, 0);
        
        // Grid
        glBegin(GL_LINES); glColor3f(0.3f, 0.3f, 0.3f);
        for(int i=-20; i<=20; i+=2) { glVertex3f(i,0,-20); glVertex3f(i,0,20); glVertex3f(-20,0,i); glVertex3f(20,0,i); }
        glEnd();
        
        // Render Agents
        for(int i=0; i<state.agent_count; i++) {
            Agent* a = &state.agents[i];
            float r = (strcmp(a->status, "active")==0) ? 0.0f : 1.0f; // Green active, Red idle
            float g = (strcmp(a->status, "active")==0) ? 1.0f : 0.5f;
            draw_cube(a->x, 0.5f, a->z, r, g, 0.0f);
        }
        
        cam_angle += 0.2f;
        SDL_GL_SwapWindow(win); SDL_Delay(16);
    }
    return 0;
}

#else
// --- GENERATOR MODE (Linux/Headless) ---
int main() {
    printf("📡 Empire Generator Starting...\n");
    EmpireState state;
    empire_load("empire.json", &state);
    printf("   Loaded: %s (%d agents)\n", state.empire_name, state.agent_count);
    
    empire_generate_site(&state);
    printf("   ✅ Dashboard generated in /output/index.html\n");
    return 0;
}
#endif
