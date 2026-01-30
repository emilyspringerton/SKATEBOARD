#define SDL_MAIN_HANDLED
#include <stdio.h>
#include "shank.h"
#include "input.h"
#include "net.h"
#include "font.h"
#include "particles.h"
#include <GL/glu.h>
#include "../../../packages/map/map.h"
#include "ipc_client.c" // DIRECT INCLUDE FOR BUILD SIMPLICITY

// ... [Existing Includes] ...

GameContext game;
GameMap level;

// ... [Existing Render Functions] ...

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    game.window = SDL_CreateWindow("SHANK PIT ONLINE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    game.gl_context = SDL_GL_CreateContext(game.window);
    init_opengl(); input_init(&game); net_init(&game, "127.0.0.1"); map_init(&level); particles_init();
    
    // --- PROOF OF FUN HOOK ---
    printf("[SHANKPIT] Initializing Mining Bridge...\n");
    ipc_connect_and_handshake();
    // -------------------------

    game.running = true; Uint32 last_time = SDL_GetTicks();
    int frame_counter = 0;

    while (game.running) {
        Uint32 current_time = SDL_GetTicks(); float delta_time = (current_time - last_time) / 1000.0f; last_time = current_time;
        
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) game.running = 0;
            // ... [Input Handling] ...
        }

        input_update(&game, delta_time); 
        net_send_input(&game); 
        net_receive_state(&game); 
        render();

        // --- PROOF OF FUN HEARTBEAT ---
        frame_counter++;
        if (frame_counter % 60 == 0) { // Approx 1 Hz
            float entropy = 0.8f; // TODO: Calculate real entropy from input.c
            float fps = 1.0f / (delta_time + 0.0001f);
            ipc_send_heartbeat(fps, entropy);
        }
        // ------------------------------
    }
    // ... [Cleanup] ...
    return 0;
}