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

// VEHICLE TUNING
#define VEH_SUSPENSION_LEN 1.2f
#define VEH_SPRING_FORCE 0.35f
#define VEH_DAMPER 0.1f
#define VEH_ACCEL 0.08f
#define VEH_FRICTION_LAT 0.92f // Drift
#define VEH_FRICTION_FWD 0.99f
#define VEH_TURN_SPEED 2.5f
#define VEH_GRAVITY 0.02f

float loc_rand_f() { return ((float)(rand()%1000)/500.0f) - 1.0f; }

// Math Helpers
Vec3 vec_add(Vec3 a, Vec3 b) { return (Vec3){a.x+b.x, a.y+b.y, a.z+b.z}; }
Vec3 vec_sub(Vec3 a, Vec3 b) { return (Vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
Vec3 vec_mul(Vec3 a, float s) { return (Vec3){a.x*s, a.y*s, a.z*s}; }
float vec_dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec3 vec_cross(Vec3 a, Vec3 b) { return (Vec3){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; }

// Rotate vector by Yaw
Vec3 rotate_y(Vec3 v, float angle_deg) {
    float r = angle_deg * 3.14159f / 180.0f;
    float c = cosf(r); float s = sinf(r);
    return (Vec3){ v.x*c + v.z*s, v.y, -v.x*s + v.z*c };
}

void local_init() {
    map_init(&local_level);
    memset(&local_state, 0, sizeof(local_state));
    // Spawn Player
    local_state.players[0].active = 1; 
    local_state.players[0].health = 100; 
    local_state.players[0].pos.y = 5.0f; 
    local_state.players[0].current_weapon = 1;
    for(int j=0; j<5; j++) local_state.players[0].ammo[j] = WPN_STATS[j].ammo_max;
    
    // Spawn Warthog
    local_state.vehicle.active = 1;
    local_state.vehicle.pos = (Vec3){5.0f, 5.0f, 5.0f};
    local_state.vehicle.driver_id = -1;
}

// 4-Point Suspension Physics
void update_vehicle() {
    VehicleState *v = &local_state.vehicle;
    if (!v->active) return;

    // Gravity
    v->vel.y -= VEH_GRAVITY;

    // Chassis Basis Vectors (Simplified: Yaw only for movement, Pitch/Roll visual mostly)
    // To do true "one wheel grip", we need to apply force at offsets.
    // For this retro engine, we will cheat:
    // We cast 4 rays. The number of hits determines traction. 
    // The average hit height determines tilt.
    
    Vec3 fwd = rotate_y((Vec3){0,0,1}, v->yaw);
    Vec3 right = rotate_y((Vec3){1,0,0}, v->yaw);
    
    // Wheel Offsets (Relative to center)
    Vec3 corners[4];
    float w = 1.2f; float l = 1.8f;
    corners[0] = vec_add(vec_mul(fwd, l), vec_mul(right, w)); // FL
    corners[1] = vec_add(vec_mul(fwd, l), vec_mul(right, -w)); // FR
    corners[2] = vec_add(vec_mul(fwd, -l), vec_mul(right, w)); // RL
    corners[3] = vec_add(vec_mul(fwd, -l), vec_mul(right, -w)); // RR

    int wheels_grounded = 0;
    float avg_y = 0;
    
    for(int i=0; i<4; i++) {
        Vec3 wheel_pos = vec_add(v->pos, corners[i]);
        // Raycast Down
        // Check map height at this X/Z
        // Hack: Since we don't have a generic "get_height" function in map.c, we assume floor is at Y=0 for grid or Y=height for blocks.
        // We will perform a short raycast.
        
        Vec3 down = {0, -1, 0};
        if (map_ray_cast(&local_level, wheel_pos, down, VEH_SUSPENSION_LEN)) {
            // HIT! Apply Spring Force
            // This is a simplified spring. Real spring needs compression delta.
            // Just push UP.
            v->vel.y += VEH_SPRING_FORCE * 0.25f; // Additive lift
            wheels_grounded++;
            
            // Visual Tilt Logic (Fake Torque)
            // If FL hits, roll right, pitch up.
            if (i==0) { v->roll -= 0.5f; v->pitch += 0.5f; }
            if (i==1) { v->roll += 0.5f; v->pitch += 0.5f; }
            if (i==2) { v->roll -= 0.5f; v->pitch -= 0.5f; }
            if (i==3) { v->roll += 0.5f; v->pitch -= 0.5f; }
        }
    }
    
    // Damping
    v->roll *= 0.90f;
    v->pitch *= 0.90f;
    
    // Drive Physics (Only if grounded)
    if (wheels_grounded > 0) {
        // Friction
        Vec3 local_vel;
        // Project velocity onto local axis
        float fwd_speed = vec_dot(v->vel, fwd);
        float side_speed = vec_dot(v->vel, right);
        
        // Apply friction
        fwd_speed *= VEH_FRICTION_FWD;
        side_speed *= VEH_FRICTION_LAT; // Drift factor
        
        // Reconstruct velocity
        v->vel = vec_add(vec_mul(fwd, fwd_speed), vec_mul(right, side_speed));
        v->vel.y *= 0.98f; // Vertical damping
    }
    
    // Integrate
    v->pos = vec_add(v->pos, v->vel);
    
    // Floor clamp (Safety)
    if (v->pos.y < 0.5f) { v->pos.y = 0.5f; v->vel.y = 0; }
}

// DRIVING INPUT
void drive_vehicle(float gas, float steer_yaw) {
    VehicleState *v = &local_state.vehicle;
    if (!v->active) return;
    
    // Steering: Align Vehicle Yaw to Camera Yaw (Steer_Yaw)
    // Find shortest angle difference
    float diff = steer_yaw - v->yaw;
    while (diff < -180) diff += 360;
    while (diff > 180) diff -= 360;
    
    // Turn towards camera look
    if (fabs(diff) > 2.0f) {
        if (diff > 0) v->yaw += VEH_TURN_SPEED;
        else v->yaw -= VEH_TURN_SPEED;
    }
    
    // Gas: Move in Vehicle Forward direction
    if (fabs(gas) > 0.1f) {
        Vec3 fwd = rotate_y((Vec3){0,0,1}, v->yaw);
        v->vel = vec_add(v->vel, vec_mul(fwd, gas * VEH_ACCEL));
    }
}

void local_update(float dt, float fwd, float strafe, float yaw, float pitch, int shoot, int weapon, int jump, int crouch, int reload, int use) {
    local_state.server_tick++;
    
    // 1. Update Physics for ALL entities
    update_vehicle(); // Physics always runs (gravity etc)
    
    PlayerState *p = &local_state.players[local_pid];
    
    // 2. Handle Vehicle Entry/Exit
    if (use && local_state.vehicle.active) {
        // Check distance
        float d = (p->pos.x - local_state.vehicle.pos.x)*(p->pos.x - local_state.vehicle.pos.x) + 
                  (p->pos.z - local_state.vehicle.pos.z)*(p->pos.z - local_state.vehicle.pos.z);
        
        if (p->in_vehicle) {
            // Eject
            p->in_vehicle = 0;
            local_state.vehicle.driver_id = -1;
            p->pos.y += 2.0f; 
            p->vel.y = 0.5f; // Jump out
        } 
        else if (d < 16.0f && local_state.vehicle.driver_id == -1) {
            // Enter
            p->in_vehicle = 1;
            local_state.vehicle.driver_id = local_pid;
        }
    }

    if (p->in_vehicle) {
        // DRIVING MODE
        // Lock player to car
        p->pos = local_state.vehicle.pos;
        p->yaw = yaw; // Player looks around
        p->pitch = pitch;
        
        // Input drives car
        if (local_state.vehicle.driver_id == local_pid) {
            drive_vehicle(fwd, yaw); // W/S = Gas, Camera = Steer
        }
    } else {
        // INFANTRY MODE (Standard FPS)
        // ... (Previous logic code) ...
        // [Simplified for brevity in this patch, assuming previous logic persists or is copied]
        // RE-IMPLEMENTING BASIC MOVEMENT FOR COMPLETENESS:
        
        p->yaw = yaw; p->pitch = pitch;
        if (weapon >= 0 && weapon < 5) p->current_weapon = weapon;
        
        // Friction
        p->vel.x *= 0.82f; p->vel.z *= 0.82f;
        
        // Accel
        float r = -yaw * (3.14159f/180.0f);
        float fwd_x = sinf(r); float fwd_z = -cosf(r); 
        float right_x = cosf(r); float right_z = sinf(r);
        float wish_x = (fwd_x * fwd) + (right_x * strafe); 
        float wish_z = (fwd_z * fwd) + (right_z * strafe);
        
        p->vel.x += wish_x * 0.15f; p->vel.z += wish_z * 0.15f;
        
        // Gravity/Jump
        p->vel.y -= 0.02f;
        if (jump && p->pos.y <= 0.1f) p->vel.y = 0.4f;
        
        p->pos.x += p->vel.x; p->pos.y += p->vel.y; p->pos.z += p->vel.z;
        if (p->pos.y < 0) { p->pos.y = 0; p->vel.y = 0; }
        
        map_resolve_collision(&local_level, &p->pos, &p->vel);
    }
}
#endif
