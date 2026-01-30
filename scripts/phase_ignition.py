# Meta-Layer for Code Generation
import os

print("🔥 Generating Empire of the Worlds (Visuals)...")

# 1. Define The Grid Code (C Source)
# Note: We use raw strings and explicit newlines to keep it clean.
main_c = r'''#include <SDL.h>
#include <math.h>
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define GRID_SIZE 20
#define GRID_SPACING 40.0f

typedef struct { float x, y, z; } Point3D;

// Simple Projection
void project(Point3D p, float *sx, float *sy, float cam_h) {
    float fov = 400.0f;
    float scale = fov / (p.z + 400.0f); // Depth scale
    *sx = (p.x * scale) + (SCREEN_W / 2);
    *sy = ((p.y - cam_h) * scale) + (SCREEN_H / 2);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* win = SDL_CreateWindow("EMPIRE OF THE WORLDS: GRID", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    float rotation = 0.0f;
    float cam_height = 100.0f;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP) cam_height += 10.0f;
                if (e.key.keysym.sym == SDLK_DOWN) cam_height -= 10.0f;
            }
        }

        rotation += 0.02f;

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 255); // Retro Green

        // Draw Grid
        for (int i = -GRID_SIZE; i <= GRID_SIZE; i++) {
            // Horizontal Lines
            Point3D p1 = { -GRID_SIZE * GRID_SPACING, 100.0f, i * GRID_SPACING };
            Point3D p2 = { GRID_SIZE * GRID_SPACING, 100.0f, i * GRID_SPACING };
            
            // Rotate P1
            float x1 = p1.x * cos(rotation) - p1.z * sin(rotation);
            float z1 = p1.x * sin(rotation) + p1.z * cos(rotation);
            // Rotate P2
            float x2 = p2.x * cos(rotation) - p2.z * sin(rotation);
            float z2 = p2.x * sin(rotation) + p2.z * cos(rotation);

            float sx1, sy1, sx2, sy2;
            project((Point3D){x1, p1.y, z1}, &sx1, &sy1, cam_height);
            project((Point3D){x2, p2.y, z2}, &sx2, &sy2, cam_height);
            
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
'''

# 2. Build Directory Structure
os.makedirs("apps/lobby/src", exist_ok=True)
os.makedirs("packages/common", exist_ok=True)
os.makedirs("packages/simulation", exist_ok=True)

# 3. Write Files
with open("apps/lobby/src/main.c", "w") as f:
    f.write(main_c)

# Write dummy headers to satisfy Includes if needed
with open("packages/common/physics.h", "w") as f:
    f.write("// Physics placeholder")
with open("packages/simulation/local_game.h", "w") as f:
    f.write("// Sim placeholder")

print("📄 Grid Source Generated.")
