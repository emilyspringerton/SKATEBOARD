#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

// SkateChain Fishing Constants
#define REEL_GRAVITY 0.005f
#define REEL_POWER 0.015f

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("SKATECHAIN // FISHING", 100, 100, 800, 600, 0);
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    float bar_pos = 0.5f;
    float fish_pos = 0.5f;
    float progress = 0.3f;
    int is_reeling = 0;

    printf("🎣 Fishing Engine Online. Use [SPACE] to reel.\n");

    int running = 1;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) is_reeling = 1;
            if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_SPACE) is_reeling = 0;
        }

        // Physics
        if(is_reeling) bar_pos -= REEL_POWER;
        else bar_pos += REEL_GRAVITY;

        // Keep bar in bounds
        if(bar_pos < 0.1f) bar_pos = 0.1f;
        if(bar_pos > 0.9f) bar_pos = 0.9f;

        // Fish AI (Mock)
        fish_pos += sinf(SDL_GetTicks() * 0.005f) * 0.02f;

        // Collision logic
        if(fabs(fish_pos - bar_pos) < 0.1f) progress += 0.002f;
        else progress -= 0.001f;

        if(progress >= 1.0f) {
            printf("🎉 CAUGHT! Submitting receipt to SkateChain...\n");
            progress = 0.3f; // Reset for next loop
        }

        // Render Background (Deep Water)
        SDL_SetRenderDrawColor(rend, 10, 30, 60, 255);
        SDL_RenderClear(rend);

        // Render Bar
        SDL_SetRenderDrawColor(rend, 0, 255, 100, 255);
        SDL_Rect r_bar = {700, (int)(bar_pos * 600), 20, 60};
        SDL_RenderFillRect(rend, &r_bar);

        // Render Fish
        SDL_SetRenderDrawColor(rend, 255, 200, 0, 255);
        SDL_Rect r_fish = {705, (int)(fish_pos * 600), 10, 10};
        SDL_RenderFillRect(rend, &r_fish);

        SDL_RenderPresent(rend);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
