#ifndef TURTLE_H
#define TURTLE_H

#include "protocol.h"

typedef struct {
    Vec3 pos;
    float angle;
    bool pen_down;
    Vec3 color;
} Turtle;

// Move the turtle forward and return the new position
Vec3 turtle_forward(Turtle *t, float distance) {
    float rad = t->angle * (3.14159f / 180.0f);
    t->pos.x += cosf(rad) * distance;
    t->pos.z += sinf(rad) * distance;
    return t->pos;
}

#endif
