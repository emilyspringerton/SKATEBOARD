#ifndef EMPIRE_H
#define EMPIRE_H

typedef struct {
    char name[32];
    char task[32];
    int xp;
    char status[16];
    float x, z; // Position in the 3D world
} Agent;

typedef struct {
    char empire_name[64];
    long gold;
    long hourly_rate;
    int agent_count;
    Agent agents[64];
} EmpireState;

// Functions
void empire_load(const char* filename, EmpireState* state);
void empire_generate_site(EmpireState* state);

#endif
