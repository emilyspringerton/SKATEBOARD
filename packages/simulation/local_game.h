#ifndef LOCAL_GAME_H
#define LOCAL_GAME_H
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h" 
#include "map.h"

extern ServerState local_state; 
extern int local_pid;
static GameMap local_level;

#define GRAVITY 0.02f
#define JUMP_POWER 0.42f
#define MAX_SPEED 0.8f
#define AIR_FRICTION 0.99f 
#define ACCEL 0.06f     

float map_resolve_collision(GameMap *map, Vec3 *pos, Vec3 *vel);

float loc_rand_f() { return ((float)(rand()%1000)/500.0f) - 1.0f; }
Vec3 vec_add(Vec3 a, Vec3 b) { return (Vec3){a.x+b.x, a.y+b.y, a.z+b.z}; }
Vec3 vec_sub(Vec3 a, Vec3 b) { return (Vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
Vec3 vec_mul(Vec3 a, float s) { return (Vec3){a.x*s, a.y*s, a.z*s}; }
float vec_dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec3 rotate_y(Vec3 v, float angle_deg) {
    float r = angle_deg * 3.14159f / 180.0f;
    float c = cosf(r); float s = sinf(r);
    return (Vec3){ v.x*c + v.z*s, v.y, -v.x*s + v.z*c };
}
Vec3 loc_get_aim(float yaw, float pitch, float spread) {
    float r = -yaw * (3.14159f/180.0f); float rp = pitch * (3.14159f/180.0f);
    Vec3 fwd = { sin(r)*cos(rp), sin(rp), -cos(r)*cos(rp) };
    if (spread > 0) { fwd.x+=loc_rand_f()*spread; fwd.y+=loc_rand_f()*spread; fwd.z+=loc_rand_f()*spread; }
    return fwd;
}
float loc_dist_sq(Vec3 a, Vec3 b) { return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z); }
int loc_ray_hit(Vec3 origin, Vec3 dir, Vec3 target, float radius) {
    Vec3 oc = { target.x-origin.x, target.y-origin.y, target.z-origin.z };
    float t = (oc.x*dir.x + oc.y*dir.y + oc.z*dir.z); if (t < 0) return 0;
    Vec3 p = { origin.x + dir.x*t, origin.y + dir.y*t, origin.z + dir.z*t };
    return loc_dist_sq(p, target) < (radius*radius);
}

void local_init() {
    map_init(&local_level);
    memset(&local_state, 0, sizeof(local_state));
    local_state.players[0].active = 1; 
    local_state.players[0].health = 100; 
    local_state.players[0].pos.y = 10.0f; 
    local_state.players[0].current_weapon = 1;
    local_state.players[0].ground_friction = 0.8f; 
    
    local_state.vehicle.active = 1;
    local_state.vehicle.pos = (Vec3){60.0f, 10.0f, 0.0f}; 
    local_state.vehicle.driver_id = -1;
    local_state.vehicle.yaw = -90.0f;
}

void update_vehicle() {
    VehicleState *v = &local_state.vehicle;
    if (!v->active) return;
    v->vel.y -= 0.02f;
    Vec3 fwd = rotate_y((Vec3){0,0,1}, v->yaw);
    Vec3 right = rotate_y((Vec3){1,0,0}, v->yaw);
    Vec3 corners[4]; float w=1.2f, l=1.8f;
    corners[0] = vec_add(vec_mul(fwd, l), vec_mul(right, w));
    corners[1] = vec_add(vec_mul(fwd, l), vec_mul(right, -w));
    corners[2] = vec_add(vec_mul(fwd, -l), vec_mul(right, w));
    corners[3] = vec_add(vec_mul(fwd, -l), vec_mul(right, -w));
    int grounded = 0;
    for(int i=0; i<4; i++) {
        Vec3 wp = vec_add(v->pos, corners[i]);
        if(map_ray_cast(&local_level, wp, (Vec3){0,-1,0}, 1.5f)) {
            v->vel.y += 0.1f; grounded++;
            if(i<2) v->pitch += 0.2f; else v->pitch -= 0.2f;
        }
    }
    v->pitch *= 0.9f; v->roll *= 0.9f;
    if(grounded) {
        v->vel.x *= 0.98f; v->vel.z *= 0.98f; 
        v->vel.y *= 0.95f;
    }
    v->pos = vec_add(v->pos, v->vel);
    if(v->pos.y < 0.5f) { v->pos.y = 0.5f; v->vel.y = 0; }
}

void local_update(float dt, float fwd, float strafe, float yaw, float pitch, int shoot, int weapon, int jump, int crouch, int reload, int use) {
    local_state.server_tick++;
    update_vehicle();
    PlayerState *p = &local_state.players[local_pid];
    
    if (use && local_state.vehicle.active) {
        float dist_sq = loc_dist_sq(p->pos, local_state.vehicle.pos);
        if (p->in_vehicle) { p->in_vehicle = 0; local_state.vehicle.driver_id = -1; p->pos.y += 3.0f; p->vel.y = 0.5f; } 
        else if (dist_sq < 16.0f && local_state.vehicle.driver_id == -1) { p->in_vehicle = 1; local_state.vehicle.driver_id = local_pid; }
    }

    if (p->in_vehicle) {
        p->pos = local_state.vehicle.pos;
        p->yaw = yaw; p->pitch = pitch;
        if (fabs(fwd) > 0.1f) {
            Vec3 vfwd = rotate_y((Vec3){0,0,1}, local_state.vehicle.yaw);
            local_state.vehicle.vel = vec_add(local_state.vehicle.vel, vec_mul(vfwd, fwd * 0.05f));
        }
        float diff = yaw - local_state.vehicle.yaw;
        while(diff<-180)diff+=360; while(diff>180)diff-=360;
        local_state.vehicle.yaw += diff * 0.1f; 
    } else {
        p->yaw = yaw; p->pitch = pitch;
        if (weapon >= 0 && weapon < 5) p->current_weapon = weapon;
        
        float fric = (p->pos.y > 0.1f) ? AIR_FRICTION : p->ground_friction;
        if (fric < 0.5f) fric = 0.8f;
        p->vel.x *= fric; p->vel.z *= fric;
        
        float r = -yaw * (3.14159f/180.0f);
        float fwd_x = sinf(r); float fwd_z = -cosf(r); float right_x = cosf(r); float right_z = sinf(r);
        float wish_x = (fwd_x * fwd) + (right_x * strafe); float wish_z = (fwd_z * fwd) + (right_z * strafe);
        
        p->vel.x += wish_x * ACCEL; p->vel.z += wish_z * ACCEL;
        p->vel.y -= GRAVITY;
        if (jump && p->pos.y <= 0.1f) p->vel.y = JUMP_POWER;
        
        p->pos.x += p->vel.x; p->pos.y += p->vel.y; p->pos.z += p->vel.z;
        if (p->pos.y < 0) { p->pos.y = 0; p->vel.y = 0; }
        
        float surface_fric = map_resolve_collision(&local_level, &p->pos, &p->vel);
        if (surface_fric > 0) p->ground_friction = surface_fric;
        else p->ground_friction = AIR_FRICTION;
    }
}
#endif
