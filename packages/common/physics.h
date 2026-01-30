#ifndef PHYSICS_H
#define PHYSICS_H

#include <math.h>

#define MAX_SPEED 0.42f
#define ACCEL 0.05f
#define FRICTION 0.92f
#define GRAVITY 0.018f

typedef struct { float x, y, z; } Vec3;
typedef struct { Vec3 pos, vel; float yaw, pitch; } Entity;

void phys_apply_friction(Entity *e);
void phys_accelerate(Entity *e, float wish_x, float wish_z, float target_speed, float accel_val);

#endif
