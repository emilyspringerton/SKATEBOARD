#include "map.h"
#include <math.h>
#include <stdio.h>

void map_init(GameMap *map) {
    map->wall_count = 0;
    #define ADD_WALL(x, y, z, w, h, d, cr, cg, cb)         map->walls[map->wall_count++] = (Wall){x, y, z, w, h, d, cr, cg, cb}

    ADD_WALL(0, 1, 0,  1.0f, 1.0f, 1.0f,  0.5f, 0.5f, 0.5f);
    ADD_WALL(10, 0.5f, 10,  2.0f, 0.5f, 2.0f,  0.6f, 0.4f, 0.3f);
    ADD_WALL(10, 1.5f, 10,  1.0f, 0.5f, 1.0f,  0.6f, 0.4f, 0.3f); 
    ADD_WALL( 20, 2,  0,  1.0f, 2.0f, 20.0f,  0.2f, 0.2f, 0.3f); 
    ADD_WALL(-20, 2,  0,  1.0f, 2.0f, 20.0f,  0.2f, 0.2f, 0.3f); 
    ADD_WALL( 0,  2, 20,  20.0f, 2.0f, 1.0f,  0.2f, 0.2f, 0.3f); 
    ADD_WALL( 0,  2, -20, 20.0f, 2.0f, 1.0f,  0.2f, 0.2f, 0.3f); 
}

int map_ray_cast(GameMap *map, Vec3 origin, Vec3 dir, float max_dist) {
    for (int i=0; i < map->wall_count; i++) {
        Wall *w = &map->walls[i];
        float b_min_x = w->x - w->sx; float b_max_x = w->x + w->sx;
        float b_min_y = w->y - w->sy; float b_max_y = w->y + w->sy;
        float b_min_z = w->z - w->sz; float b_max_z = w->z + w->sz;

        float t1 = (b_min_x - origin.x) / dir.x; float t2 = (b_max_x - origin.x) / dir.x;
        float t3 = (b_min_y - origin.y) / dir.y; float t4 = (b_max_y - origin.y) / dir.y;
        float t5 = (b_min_z - origin.z) / dir.z; float t6 = (b_max_z - origin.z) / dir.z;

        float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
        float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

        if (tmax < 0) continue;
        if (tmin > tmax) continue;
        if (tmin < max_dist) return 1; 
    }
    return 0; 
}

// THIS WAS MISSING OR UNDEFINED
int map_check_point(GameMap *map, float x, float y, float z) {
    for (int i=0; i < map->wall_count; i++) {
        Wall *w = &map->walls[i];
        if (x > w->x - w->sx && x < w->x + w->sx &&
            y > w->y - w->sy && y < w->y + w->sy &&
            z > w->z - w->sz && z < w->z + w->sz) {
            return 1; // Collision
        }
    }
    return 0;
}

void map_resolve_collision(GameMap *map, Vec3 *pos, Vec3 *vel) {
    float player_size = 0.5f; 
    for (int i=0; i < map->wall_count; i++) {
        Wall *w = &map->walls[i];
        if (pos->x + player_size > w->x - w->sx && pos->x - player_size < w->x + w->sx &&
            pos->z + player_size > w->z - w->sz && pos->z - player_size < w->z + w->sz &&
            pos->y < w->y + w->sy) 
        {
            float dx1 = (w->x + w->sx) - (pos->x - player_size);
            float dx2 = (pos->x + player_size) - (w->x - w->sx);
            float dz1 = (w->z + w->sz) - (pos->z - player_size);
            float dz2 = (pos->z + player_size) - (w->z - w->sz);
            
            float ox = (dx1 < dx2) ? dx1 : -dx2;
            float oz = (dz1 < dz2) ? dz1 : -dz2;

            if (fabs(ox) < fabs(oz)) { pos->x += ox; vel->x = 0; } 
            else { pos->z += oz; vel->z = 0; }
        }
    }
}
