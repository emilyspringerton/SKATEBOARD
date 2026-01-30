#include "map.h"
#include <stdlib.h>

void map_generate_canyon(GameMap *m) {
    m->count = 0;
    // Floor
    m->walls[m->count++] = (MapWall){ {-100, -1, -100}, {100, 0, 100}, 0.92f, 0x111111 };
    // Pillars
    for(int i=0; i<10; i++) {
        float x = i * 8.0f - 40.0f;
        m->walls[m->count++] = (MapWall){ {x, 0, -10}, {x+2, 10, -8}, 0.98f, 0x444466 };
    }
}

void map_resolve_collision(GameMap *m, Entity *e) {
    for(int i=0; i<m->count; i++) {
        MapWall *w = &m->walls[i];
        // Check if player is inside AABB
        if(e->pos.x > w->min.x && e->pos.x < w->max.x &&
           e->pos.z > w->min.z && e->pos.z < w->max.z &&
           e->pos.y > w->min.y && e->pos.y < w->max.y) {
               
               // Calculate overlap on each axis
               float dx1 = e->pos.x - w->min.x;
               float dx2 = w->max.x - e->pos.x;
               float dz1 = e->pos.z - w->min.z;
               float dz2 = w->max.z - e->pos.z;

               // Push out along the shortest path
               float min_x = (dx1 < dx2) ? dx1 : dx2;
               float min_z = (dz1 < dz2) ? dz1 : dz2;

               if (min_x < min_z) {
                   if (dx1 < dx2) e->pos.x = w->min.x - 0.01f;
                   else e->pos.x = w->max.x + 0.01f;
                   e->vel.x = 0;
               } else {
                   if (dz1 < dz2) e->pos.z = w->min.z - 0.01f;
                   else e->pos.z = w->max.z + 0.01f;
                   e->vel.z = 0;
               }
           }
    }
}
