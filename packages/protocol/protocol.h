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
    int id; int dmg; int rof; int cnt; float spr; int ammo_max;
} WeaponStats;

// DEFINITIVE WEAPON STATS
static WeaponStats WPN_STATS[MAX_WEAPONS] = {
    {WPN_KNIFE,   35, 20, 1, 0.0f,  0},   // Knife: Fast, Infinite Ammo
    {WPN_MAGNUM,  45, 25, 1, 0.0f,  6},   // Magnum: High Dmg, Slow
    {WPN_AR,      12, 6,  1, 0.04f, 30},  // AR: Fast, Spread
    {WPN_SHOTGUN, 8,  50, 8, 0.15f, 8},   // Shotgun: 8 pellets x 8 dmg
    {WPN_SNIPER,  90, 70, 1, 0.0f,  5}    // Sniper: 1-shot (mostly), Slow
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
    int weapon_req; // 0-4 to switch, -1 to keep
} ClientInput;

#endif
