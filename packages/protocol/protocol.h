#ifndef LEGACY_PROTOCOL_H
#define LEGACY_PROTOCOL_H

#include <stdint.h>
#include "../common/physics.h"

#define PROTOCOL_VERSION 485
#define MAX_CLIENTS 32
#define PORT 7777

// --- PACKET TYPES ---
typedef enum {
    PKT_C2S_INPUT = 1,
    PKT_S2C_STATE = 2,
    PKT_C2S_JOIN  = 3
} PacketType;

// --- DATA STRUCTURES ---
#pragma pack(push, 1)

// Sent from Client to Server every frame
typedef struct {
    uint8_t type;
    uint32_t sequence;
    float fwd;
    float strafe;
    float yaw;
    uint8_t buttons; // Bitmask for Jump, Fire, etc.
} MsgInput;

// Sent from Server to Client (The Snapshot)
typedef struct {
    uint8_t type;
    uint32_t sequence;
    uint32_t server_time;
    struct {
        uint8_t active;
        Vec3 pos;
        Vec3 vel;
        float yaw;
    } players[MAX_CLIENTS];
} MsgWorldState;

#pragma pack(pop)

#endif
