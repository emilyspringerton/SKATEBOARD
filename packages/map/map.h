#ifndef MAP_H
#define MAP_H

typedef struct {
    float x, y, z;
    float sx, sy, sz; // Scale/Size (Half-Extents)
    float r, g, b;    // Color
} Wall;

typedef struct {
    Wall walls[256];
    int wall_count;
} GameMap;

void map_init(GameMap *map);

#endif
