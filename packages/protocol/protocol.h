#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define PORT 5314 
#define MAX_CLIENTS 10
#define MAX_WEAPONS 5
#define MAX_GRENADES 64
#define MAX_VEHICLES 4
#define MATCH_DURATION (60 * 60)

#define PACKET_INPUT 1
#define PACKET_STATE 2
#define PACKET_LOBBY_CMD 3
#define PACKET_LOBBY_INFO 4

#define CMD_SEARCH 1
#define CMD_CREATE 2
#define CMD_JOIN   3
#define CMD_NAME   5
#define CMD_ENTER_VEHICLE 6

#define WPN_KNIFE 0
#define WPN_MAGNUM 1
#define WPN_AR 2
#define WPN_SHOTGUN 3
#define WPN_SNIPER 4

typedef struct { float x, y, z; } Vec3;

typedef struct {
    int id;
    int active;
    Vec3 pos; Vec3 vel;
    float yaw, pitch, roll;
    int driver_id;
} VehicleState;

typedef struct {
    int active;
    int owner_id;
    int timer;
    Vec3 pos;
    Vec3 vel;
} GrenadeState;

typedef struct {
    int active;
    char name[32];
    Vec3 pos; Vec3 vel;
    float yaw; float pitch;
    int health; int shield;
    int kills; int deaths;
    int grenades;
    int current_weapon; int ammo[MAX_WEAPONS];
    int attack_cooldown; int is_shooting;
    int hit_feedback; int color; int reload_timer;
    int in_vehicle;
    int vehicle_id;
    float ground_friction; // <-- NEW: Current traction
} PlayerState;

typedef struct {
    int server_tick;
    int match_timer;
    int player_count;
    char status_msg[64];
    PlayerState players[MAX_CLIENTS];
    VehicleState vehicles[MAX_VEHICLES];
    GrenadeState grenades[MAX_GRENADES];
} ServerState;

typedef struct {
    int type; int owner_id; int cmd_id; char data[4096];
} Packet;

typedef struct {
    float fwd; float strafe; float yaw; float pitch;
    int jump; int crouch; int shoot; int reload;
    int weapon_req; int zoom; int use;
} ClientInput;

typedef struct { int id; int dmg; int rof; int cnt; float spr; int ammo_max; float range; } WeaponStats;

static const WeaponStats WPN_STATS[MAX_WEAPONS] = {
    {WPN_KNIFE,   200, 20, 1, 0.0f,  0,  2.0f},
    {WPN_MAGNUM,   45, 25, 1, 0.0f,  6, 80.0f},
    {WPN_AR,       20,  6, 1, 0.04f, 30, 60.0f},
    {WPN_SHOTGUN, 128, 17, 8, 0.15f, 8, 15.0f},
    {WPN_SNIPER,  101, 70, 1, 0.0f,  5, 200.0f}
};

typedef struct {
    int type;        // PACKET_INPUT
    int client_id;
    float fwd; float strafe; float yaw; float pitch;
    int jump; int crouch; int shoot; int reload;
    int weapon_req; int zoom; int throw_grenade; int interact;
    int show_scores;
    int render_tick; // For server rewind (lag compensation)
} ClientPacket;

#endif
