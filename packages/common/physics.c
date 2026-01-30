#include "physics.h"

void phys_apply_friction(Entity *e) {
    e->vel.x *= FRICTION;
    e->vel.z *= FRICTION;
}

void phys_accelerate(Entity *e, float wish_x, float wish_z, float target_speed, float accel_val) {
    float current_speed = e->vel.x * wish_x + e->vel.z * wish_z;
    float add_speed = target_speed - current_speed;
    if (add_speed <= 0) return;
    
    float accel_speed = accel_val * target_speed;
    if (accel_speed > add_speed) accel_speed = add_speed;
    
    e->vel.x += accel_speed * wish_x;
    e->vel.z += accel_speed * wish_z;
}
