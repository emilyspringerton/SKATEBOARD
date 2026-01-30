# Meta-Layer for Code Generation
import os

print("🔥 Ignition: Generating All Source Code...")

# --- 1. DEFINITIONS ---

# The Empire Client (Rotating Grid)
empire_c = r"""#include <SDL.h>
#include <math.h>
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define GRID_SIZE 20
#define GRID_SPACING 40.0f

typedef struct { float x, y, z; } Point3D;

void project(Point3D p, float *sx, float *sy, float cam_h) {
    float fov = 400.0f;
    float scale = fov / (p.z + 400.0f);
    *sx = (p.x * scale) + (SCREEN_W / 2);
    *sy = ((p.y - cam_h) * scale) + (SCREEN_H / 2);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    SDL_Window* win = SDL_CreateWindow("EMPIRE: GRID", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    int running = 1;
    float rotation = 0.0f;
    float cam_h = 100.0f;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP) cam_h += 10.0f;
                if (e.key.keysym.sym == SDLK_DOWN) cam_h -= 10.0f;
            }
        }
        rotation += 0.02f;
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
        
        for (int i = -GRID_SIZE; i <= GRID_SIZE; i++) {
            Point3D p1 = { -GRID_SIZE * GRID_SPACING, 100.0f, i * GRID_SPACING };
            Point3D p2 = { GRID_SIZE * GRID_SPACING, 100.0f, i * GRID_SPACING };
            float x1 = p1.x * cos(rotation) - p1.z * sin(rotation);
            float z1 = p1.x * sin(rotation) + p1.z * cos(rotation);
            float x2 = p2.x * cos(rotation) - p2.z * sin(rotation);
            float z2 = p2.x * sin(rotation) + p2.z * cos(rotation);
            float sx1, sy1, sx2, sy2;
            project((Point3D){x1, p1.y, z1}, &sx1, &sy1, cam_h);
            project((Point3D){x2, p2.y, z2}, &sx2, &sy2, cam_h);
            SDL_RenderDrawLine(ren, (int)sx1, (int)sy1, (int)sx2, (int)sy2);
        }
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
"""

# The Fishing Client (Bar vs Fish Minigame)
fishing_c = r"""#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_W 400
#define SCREEN_H 600
#define BAR_H 100
#define FISH_SIZE 30

typedef struct { float fish_y, fish_v, bar_y, bar_v, prog; } GameState;

int main(int argc, char* argv[]) {
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    SDL_Window* win = SDL_CreateWindow("SKATECHAIN FISHING", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    
    GameState s = {300.0f, 0.0f, 300.0f, 0.0f, 30.0f};
    int running = 1;
    int mouse = 0;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_MOUSEBUTTONDOWN) mouse = 1;
            if (e.type == SDL_MOUSEBUTTONUP) mouse = 0;
        }

        s.fish_v += ((float)(rand() % 200 - 100) / 100.0f);
        s.fish_v *= 0.95f;
        s.fish_y += s.fish_v;
        if (s.fish_y < 0) { s.fish_y = 0; s.fish_v *= -1; }
        if (s.fish_y > SCREEN_H - FISH_SIZE) { s.fish_y = SCREEN_H - FISH_SIZE; s.fish_v *= -1; }

        if (mouse) s.bar_v -= 1.5f; else s.bar_v += 1.0f;
        s.bar_v *= 0.9f;
        s.bar_y += s.bar_v;
        if (s.bar_y < 0) { s.bar_y = 0; s.bar_v = 0; }
        if (s.bar_y > SCREEN_H - BAR_H) { s.bar_y = SCREEN_H - BAR_H; s.bar_v = 0; }

        int overlap = (s.bar_y < s.fish_y + FISH_SIZE && s.bar_y + BAR_H > s.fish_y);
        if (overlap) s.prog += 0.3f; else s.prog -= 0.1f;
        if (s.prog < 0) s.prog = 0; if (s.prog > 100) s.prog = 100;

        SDL_SetRenderDrawColor(ren, 20, 20, 40, 255);
        SDL_RenderClear(ren);
        
        SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
        SDL_Rect bg = {350, 50, 20, 500};
        SDL_RenderFillRect(ren, &bg);
        
        SDL_SetRenderDrawColor(ren, (overlap ? 0 : 255), 255, 0, 255);
        int h = (int)(s.prog * 5.0f);
        SDL_Rect fg = {350, 550 - h, 20, h};
        SDL_RenderFillRect(ren, &fg);

        SDL_SetRenderDrawColor(ren, 100, 200, 255, 100);
        SDL_Rect bar = {100, (int)s.bar_y, 200, BAR_H};
        SDL_RenderFillRect(ren, &bar);

        SDL_SetRenderDrawColor(ren, 255, 100, 50, 255);
        SDL_Rect fish = {180, (int)s.fish_y, 40, 30};
        SDL_RenderFillRect(ren, &fish);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
"""

# --- 2. EXECUTION ---

# Create Directory Structure
dirs = ["apps/lobby/src", "apps/fishing/src", "packages/common", "packages/simulation"]
for d in dirs:
    os.makedirs(d, exist_ok=True)
    print(f"✅ Directory Verified: {d}")

# Write Files
with open("apps/lobby/src/main.c", "w") as f:
    f.write(empire_c)
print("📄 Generated: Empire Client")

with open("apps/fishing/src/main.c", "w") as f:
    f.write(fishing_c)
print("📄 Generated: Fishing Client")

with open("packages/common/physics.h", "w") as f:
    f.write("#define GRAVITY 9.81f\n")

