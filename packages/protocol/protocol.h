#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define PORT 5000
#define MAX_CLIENTS 10
#define MAX_WEAPONS 5

// Weapon IDs
#define WPN_KNIFE 0
#define WPN_MAGNUM 1
#define WPN_AR 2
#define WPN_SHOTGUN 3
#define WPN_SNIPER 4

typedef struct { float x, y, z; } Vec3;

typedef struct {
    int id; int dmg; int rof; int cnt; float spr; int ammo_max; float range;
} WeaponStats;

// UPDATED WEAPON BALANCE (Phase 89)
static WeaponStats WPN_STATS[MAX_WEAPONS] = {
    {WPN_KNIFE,   45, 20, 1,  0.0f,   0,  3.5f},   // Knife
    {WPN_MAGNUM,  45, 25, 1,  0.0f,   6,  200.0f}, // Magnum
    {WPN_AR,      12, 6,  1,  0.04f,  30, 200.0f}, // AR
    // SHOTGUN BUFF: 12 Pellets * 10 Dmg = 120 Potential. Wider Spread (0.22).
    {WPN_SHOTGUN, 10, 50, 12, 0.22f,  8,  70.0f},  
    {WPN_SNIPER,  95, 70, 1,  0.0f,   5,  500.0f}  // Sniper
};

typedef struct {
    int active;
    Vec3 pos; Vec3 vel;
    float yaw; float pitch;
    int health; int kills;
    int current_weapon; int ammo[MAX_WEAPONS];
    int attack_cooldown; int is_shooting;
    int hit_feedback; int color; int reload_timer; 
} PlayerState;

typedef struct {
    int server_tick;
    PlayerState players[MAX_CLIENTS];
} ServerState;

#define PACKET_INPUT 1
#define PACKET_STATE 2

typedef struct {
    int type;
    char data[4096];
} Packet;

typedef struct {
    float fwd; float strafe; float yaw; float pitch;
    int jump; int crouch; int shoot; int reload;
    int weapon_req; 
    int zoom; 
} ClientInput;

#endif
