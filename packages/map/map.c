#include <math.h>
#include "map.h" 

void map_init(GameMap *map) {
    map->wall_count = 0;
    
    // 1. THE FLOOR (Red Desert Sand)
    // ID 1 = Base Floor (Medium Grip)
    map->walls[map->wall_count++] = (Wall){1, 0, -1.0f, 0, 300.0f, 2.0f, 300.0f, 0.6f, 0.3f, 0.1f, 0.85f}; 

    srand(2077); // Seed for Canyon

    // 2. THE MESAS (Towering Red Rocks)
    // ID 2 = High Grip Rock (Climbable)
    for (int i=0; i<50; i++) {
        float angle = (i / 50.0f) * 6.28f;
        float dist = 50.0f + (rand()%20); 
        float x = sinf(angle) * dist;
        float z = cosf(angle) * dist;
        float h = 15.0f + (rand()%15); 
        float w = 10.0f + (rand()%10);
        
        map->walls[map->wall_count++] = (Wall){
            2, 
            x, h/2.0f - 2.0f, z, 
            w, h, w, 
            0.5f, 0.2f, 0.1f, 0.80f // GRIPPY (0.80 friction)
        };
    }

    // 3. THE BOULDERS (Slippery Scree)
    // ID 3 = Slippery Rock (Ice Physics)
    for(int i=0; i<40; i++) {
        float angle = (i / 40.0f) * 6.28f;
        float dist = 40.0f - (rand()%10);
        float x = sinf(angle) * dist;
        float z = cosf(angle) * dist;
        float h = 2.0f + (rand() % 4);
        
        map->walls[map->wall_count++] = (Wall){
            3, 
            x, h/2.0f, z, 
            3.0f, h, 3.0f, 
            0.4f, 0.4f, 0.4f, 0.98f // SLIPPERY (0.98 friction)
        };
    }
    
    // 4. Central Outpost
    map->walls[map->wall_count++] = (Wall){2, 0, 1.0f, 0, 10.0f, 2.0f, 10.0f, 0.4f, 0.3f, 0.2f, 0.85f};
    map->walls[map->wall_count++] = (Wall){2, 0, 3.0f, 0, 6.0f, 2.0f, 6.0f, 0.4f, 0.3f, 0.2f, 0.85f};
}

int map_ray_cast(GameMap *map, Vec3 origin, Vec3 dir, float max_dist) {
    for(int i=0; i<map->wall_count; i++) {
        Wall *w = &map->walls[i];
        float min_x = w->x - w->sx/2; float max_x = w->x + w->sx/2;
        float min_y = w->y - w->sy/2; float max_y = w->y + w->sy/2;
        float min_z = w->z - w->sz/2; float max_z = w->z + w->sz/2;

        float t1 = (min_x - origin.x)/dir.x; float t2 = (max_x - origin.x)/dir.x;
        float t3 = (min_y - origin.y)/dir.y; float t4 = (max_y - origin.y)/dir.y;
        float t5 = (min_z - origin.z)/dir.z; float t6 = (max_z - origin.z)/dir.z;

        float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
        float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

        if (tmax < 0 || tmin > tmax) continue;
        if (tmin < max_dist) return 1;
    }
    return 0;
}

float map_resolve_collision(GameMap *map, Vec3 *pos, Vec3 *vel) {
    float player_w = 0.6f;
    float player_h = 1.8f; 
    float detected_friction = -1.0f; 

    for(int i=0; i<map->wall_count; i++) {
        Wall *w = &map->walls[i];
        float half_sx = w->sx/2 + player_w/2;
        float half_sy = w->sy/2 + player_h/2;
        float half_sz = w->sz/2 + player_w/2;
        
        float dx = pos->x - w->x;
        float dy = (pos->y + player_h/2) - w->y; 
        float dz = pos->z - w->z;
        
        if (fabs(dx) < half_sx && fabs(dz) < half_sz && fabs(dy) < half_sy) {
            float wall_top = w->y + w->sy/2;
            
            if (vel->y <= 0 && pos->y >= (wall_top - 0.2f)) {
                pos->y = wall_top;
                vel->y = 0;
                detected_friction = w->friction; 
            } else {
                float pen_x = half_sx - fabs(dx);
                float pen_z = half_sz - fabs(dz);
                if (pen_x < pen_z) pos->x = (dx > 0) ? w->x + half_sx : w->x - half_sx;
                else pos->z = (dz > 0) ? w->z + half_sz : w->z - half_sz;
            }
        }
    }
    
    if (pos->y < -20.0f) { pos->y = 10.0f; pos->x = 0; pos->z = 0; vel->y = 0; }
    return detected_friction;
}

int map_check_point(GameMap *m, float x, float y, float z) { return (y < 0); }
