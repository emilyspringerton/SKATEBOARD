#include "map.h"
#include <stdlib.h>

void map_generate_canyon(GameMap *m) {
    m->count = 0;
    
    // 1. The Floor (High Friction)
    m->walls[m->count++] = (MapWall){ {-100, -1, -100}, {100, 0, 100}, 0.92f, 0x111111 };
    
    // 2. Procedural Mesas (Golden Ratio Placement)
    for(int i=0; i<20; i++) {
        float angle = i * PHI * 2.0f * 3.14159f;
        float dist = 10.0f + (i * 2.0f);
        float x = cosf(angle) * dist;
        float z = sinf(angle) * dist;
        
        m->walls[m->count++] = (MapWall){ 
            {x-2, 0, z-2}, {x+2, (float)(5 + (i%5)), z+2}, 
            0.98f, 0x444466 
        };
    }
}

void map_resolve_collision(GameMap *m, Entity *e) {
    for(int i=0; i<m->count; i++) {
        MapWall *w = &m->walls[i];
        // Simple AABB Check
        if(e->pos.x > w->min.x && e->pos.x < w->max.x &&
           e->pos.z > w->min.z && e->pos.z < w->max.z &&
           e->pos.y > w->min.y && e->pos.y < w->max.y) {
               // Push out logic (simplified for parity)
               e->pos.y = w->max.y;
               e->vel.y = 0;
           }
    }
}
