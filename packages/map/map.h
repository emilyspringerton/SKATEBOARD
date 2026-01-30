#ifndef MAP_H
#define MAP_H

#include "physics.h"

#define MAX_WALLS 1024
#define PHI 1.61803398875f

typedef struct {
    Vec3 min, max;
    float friction;
    uint32_t color;
} MapWall;

typedef struct {
    MapWall walls[MAX_WALLS];
    int count;
} GameMap;

void map_generate_canyon(GameMap *m);
void map_resolve_collision(GameMap *m, Entity *e);

#endif
