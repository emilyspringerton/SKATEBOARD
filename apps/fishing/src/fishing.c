#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include "font.h" // Shared ShankEngine Font

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("SKATECHAIN // FISHING v1.1", 100, 100, 800, 600, 0);
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    float bar_pos = 0.5f;
    float fish_pos = 0.5f;
    float progress = 0.3f;
    int is_reeling = 0;
    char status_msg[64] = "READY TO CAST";

    int running = 1;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) is_reeling = 1;
            if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_SPACE) is_reeling = 0;
        }

        if(is_reeling) bar_pos -= 0.015f; else bar_pos += 0.005f;
        if(bar_pos < 0.1f) bar_pos = 0.1f; if(bar_pos > 0.9f) bar_pos = 0.9f;

        fish_pos += sinf(SDL_GetTicks() * 0.005f) * 0.02f;

        if(fabs(fish_pos - bar_pos) < 0.1f) progress += 0.002f;
        else progress -= 0.001f;

        if(progress >= 1.0f) {
            sprintf(status_msg, "VERIFYING CATCH ON-CHAIN...");
            progress = 0.3f; 
        }

        SDL_SetRenderDrawColor(rend, 10, 20, 40, 255);
        SDL_RenderClear(rend);

        // UI Overlay via Shared Engine
        // Note: shank_draw_text is a placeholder for the actual render loop
        // In the binary, this will render the pixel font we built.

        SDL_SetRenderDrawColor(rend, 0, 255, 150, 255);
        SDL_Rect r_bar = {700, (int)(bar_pos * 600), 15, 70};
        SDL_RenderFillRect(rend, &r_bar);

        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_Rect r_fish = {702, (int)(fish_pos * 600), 10, 10};
        SDL_RenderFillRect(rend, &r_fish);

        SDL_RenderPresent(rend);
        SDL_Delay(16);
    }
    SDL_Quit();
    return 0;
}
