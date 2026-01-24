#include <math.h>
#include "map.h" 

void map_init(GameMap *map) {
    map->wall_count = 0;
    
    // 1. The Floor (Gray Arena Base)
    map->walls[map->wall_count++] = (Wall){1, 0, -1.0f, 0, 60.0f, 1.0f, 60.0f, 0.2f, 0.2f, 0.2f, 0.0f};

    // 2. Outer Walls (Blue-ish boundaries)
    map->walls[map->wall_count++] = (Wall){2,  30.0f, 2.0f, 0.0f,  1.0f, 5.0f, 60.0f, 0.1f, 0.1f, 0.3f, 0.0f}; // Right
    map->walls[map->wall_count++] = (Wall){3, -30.0f, 2.0f, 0.0f,  1.0f, 5.0f, 60.0f, 0.1f, 0.1f, 0.3f, 0.0f}; // Left
    map->walls[map->wall_count++] = (Wall){4,  0.0f, 2.0f,  30.0f, 60.0f, 5.0f, 1.0f, 0.1f, 0.1f, 0.3f, 0.0f}; // Front
    map->walls[map->wall_count++] = (Wall){5,  0.0f, 2.0f, -30.0f, 60.0f, 5.0f, 1.0f, 0.1f, 0.1f, 0.3f, 0.0f}; // Back

    // 3. Random Cover Boxes (Orange/Rust color)
    // Deterministic seed for consistency across clients
    srand(12345); 
    for(int i=0; i<15; i++) {
        float x = (rand() % 40) - 20.0f;
        float z = (rand() % 40) - 20.0f;
        float h = 1.0f + ((rand() % 20) / 10.0f); // Height 1.0 to 3.0
        
        map->walls[map->wall_count++] = (Wall){
            10 + i, 
            x, h/2.0f - 0.5f, z, 
            2.0f, h, 2.0f, 
            0.6f, 0.3f, 0.1f, 0.0f
        };
    }
    
    // 4. Central Tower (The "King of the Hill" spot)
    map->walls[map->wall_count++] = (Wall){99, 0.0f, 1.0f, 0.0f, 4.0f, 2.0f, 4.0f, 0.3f, 0.3f, 0.3f, 0.0f};
}

// Simple AABB Raycast (Checking if bullet hits wall)
int map_ray_cast(GameMap *map, Vec3 origin, Vec3 dir, float max_dist) {
    for(int i=0; i<map->wall_count; i++) {
        Wall *w = &map->walls[i];
        // Simplified AABB Check vs Ray
        // In full engine we use slab method, here we check simple distance to centers for speed
        float dx = fabs(origin.x - w->x);
        float dy = fabs(origin.y - w->y);
        float dz = fabs(origin.z - w->z);
        if (dx < w->sx/2.0f && dy < w->sy/2.0f && dz < w->sz/2.0f) return 1; // Start inside
    }
    return 0; // Miss
}

void map_resolve_collision(GameMap *map, Vec3 *pos, Vec3 *vel) {
    // Placeholder collision logic
    if (pos->y < 0) { pos->y = 0; vel->y = 0; }
}
