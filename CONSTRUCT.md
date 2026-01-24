# FARTCO MONOREPO CONTEXT

## STRUCTURE
```text
./
    deploy_linux.sh
    README.md
    apps/
        shank-fps/
            src/
                input.h
                net.h
                shank.h
                particles.h
                net.c
                input.c
                font.h
                main.c
        fishing/
            src/
                fishing.c
        lobby/
            src/
                main.c
        server/
            src/
                main.c
        bot_client/
            swarm_launcher.py
            bot_runner.py
        sitegen/
            main.h
            content.json
            main.c
            output/
        empire/
            empire.json
            main.h
            font.h
            main.c
            output/
    services/
        master-server/
        game-server/
            src/
                heartbeat.h
                server.c
        skate-node/
            chain/
            consensus/
                pow/
                pol/
            bridge/
            vm/
            state/
            mempool/
            miner/
            rpc/
            p2p/
            contract/
    packages/
        protocol/
            protocol.h
        assets/
        map/
            map.c
            map.h
        common/
            config_loader.h
            font.h
        simulation/
            game_physics.h
            local_game.h
    tools/
        skateboard-cli/
            cmd/
                server/
            internal/
                parser/
                db/
                git/
    ops/
        docker/
        genesis/
            genesis.json
    release_temp/
    release_monolith/
    release_multiplayer/
    utils/
        cli/
            deploy_server.py
            build_server.py
            reset_server.py
            nuclear_deploy.py
            git_sync.py
            deploy_hybrid.py
            bot_patrol.py
    .ssh/
    release_server/
        run.sh
    release_client/
    releases/
        Fishing_v1_1/
        ShankPit_v46/
        ShankPit_v47_Canyon/
        ShankPit_v159_Restored/
```

## SOURCE

### FILE: ./deploy_linux.sh
```cpp
#!/bin/bash
echo "🚀 DEPLOYING SHANK PIT SERVER..."

# 1. Install Deps
sudo apt-get update
sudo apt-get install -y gcc make libsdl2-dev libgl1-mesa-dev libglu1-mesa-dev screen git golang

# 2. Compile Master
echo "🛠️ Building Master Server..."
cd services/master-server
go build -o ../../bin/master_server .
cd ../..

# 3. Compile Game Server
echo "🛠️ Building Game Server..."
gcc services/game-server/src/server.c -o bin/shank_server -lm

# 4. Config
echo "⚙️ Writing Config..."
cat > bin/config.ini <<EOF
[Network]
MasterIP=127.0.0.1
MasterPort=8080
GamePort=6969
EOF

# 5. Run
echo "🔥 Launching..."
killall master_server
killall shank_server

screen -dmS master ./bin/master_server
screen -dmS game1 ./bin/shank_server

echo "✅ SERVER ONLINE. Use 'screen -r master' to view logs."

```

### FILE: ./README.md
```cpp
# 🛹 SkateChain: The Polyglot Metaverse

> **Vision:** A decentralized ecosystem connecting high-performance C game clients with a Go-based blockchain backend.
> **Tagline:** Fragment The Void.

## 🏗️ Architecture Stack

### Core Services
- [ ] **Skate Node (Go):** Custom Layer-1 Blockchain.
    - Features: Hybrid PoW/PoL Consensus, State Trie, P2P GossipSub.
- [ ] **Master Server (Go):** Authentication & Matchmaking.
    - Features: Google OAuth, Server Browser, Elo Queues.
- [ ] **Bridge Node (Go):** ZK-SNARK Prover.
    - Features: Cross-chain state root submission to Ethereum.

### Game Clients (C/SDL2)
- [ ] **ShankPit (FPS):** Quake-style arena shooter.
    - Features: 4WD Warthog Physics, Projectile Grenades, Red Rock Map.
- [ ] **Empire of the Worlds:** Automation Dashboard.
    - Features: 3D Visualizer, JSON State Loader, HTML Generator.
- [ ] **Fishing:** Chill economy loop.
    - Features: RNG Fairness, Loot Tables, Inventory.

---

## 🛠️ Setup & Installation

### Prerequisites
- GCC / MinGW (C Compiler)
- Go 1.21+
- SDL2 Development Libraries
- MySQL & Redis (Infrastructure)

### Build Instructions
- [ ] Run `make all` to compile the entire monorepo.
- [ ] Run `make services` for backend only.
- [ ] Run `make clients` for game binaries.

---

## 📅 Roadmap: Phase 1 (Foundation)

### Infrastructure
- [x] Initialize Monorepo Structure.
- [x] Build Skateboard CLI (The Factory).
- [ ] Deploy Docker Compose stack.

### Gameplay
- [x] Implement FPS Movement & Shooting.
- [x] Implement Vehicle Physics (Warthog).
- [ ] Finalize Fishing RNG Logic.
- [ ] Create Inventory UI.

### Blockchain
- [ ] Genesis Block Ceremony.
- [ ] Implement RandomX Mining.
- [ ] Implement BLS Signature Aggregation.

---

## 🌍 Empire of the Worlds Spec

### Concept
A meta-layer for managing bots and agents across digital realms.

### Data Model (`empire.json`)
- **Agents:** XP, Status, Task, Location (X/Y/Z).
- **Economy:** Gold, Hourly Rate.
- **World:** Map definition and constraints.

### Tooling
- **Visualizer:** 3D rendering of agent state.
- **Generator:** Static HTML dashboard for web view.

---

## 🔫 ShankPit FPS Spec

### Weapons
1. **Knife:** High damage, melee range.
2. **Magnum:** Precision hitscan, shield stripper.
3. **AR:** Full-auto, spread.
4. **Shotgun:** 10-pellet spread, close range.
5. **Sniper:** Infinite range, trail rendering.

### Map: Red Rock Canyon
- **Terrain:** Procedural mesas and slippery boulders.
- **Physics:** Variable friction surfaces (Rock = Grip, Ice = Slip).

```

### FILE: ./apps/shank-fps/src/input.h
```cpp
#ifndef INPUT_H
#define INPUT_H

#include "shank.h"

// Initialize Input (Hide cursor, capture mouse)
void input_init(GameContext *game);

// Process Frame Input
void input_update(GameContext *game, float delta_time);

#endif

```

### FILE: ./apps/shank-fps/src/net.h
```cpp
#ifndef NET_H
#define NET_H

#include "shank.h"
#include "../../../packages/protocol/protocol.h"

void net_init(GameContext *game, const char *ip);
void net_send_input(GameContext *game);
void net_receive_state(GameContext *game);

#endif

```

### FILE: ./apps/shank-fps/src/shank.h
```cpp
#ifndef SHANK_H
#define SHANK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdbool.h>
#include <math.h>
#include "../../../packages/protocol/protocol.h" // Include Shared Protocol

// --- Constants ---
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define FOV 70.0f

// --- Core Entities ---
typedef struct {
    Vec3 pos;
    Vec3 vel;
    float yaw;
    float pitch;
    int current_weapon;
    int health;
    int ammo[6];
} Player;

typedef struct {
    bool running;
    SDL_Window* window;
    SDL_GLContext gl_context;
    Player local_player;
    
    // NEW: Network Data
    ServerState last_server_state;
} GameContext;

#endif

```

### FILE: ./apps/shank-fps/src/particles.h
```cpp
#ifndef PARTICLES_H
#define PARTICLES_H

#include "shank.h"

typedef struct {
    Vec3 pos;
    Vec3 vel;
    float life; // 1.0 to 0.0
    float r, g, b;
} Particle;

#define MAX_PARTICLES 128
Particle particles[MAX_PARTICLES];

void particles_init() {
    for(int i=0; i<MAX_PARTICLES; i++) particles[i].life = 0;
}

void spawn_particle(Vec3 pos, float r, float g, float b) {
    for(int i=0; i<MAX_PARTICLES; i++) {
        if(particles[i].life <= 0) {
            particles[i].pos = pos;
            particles[i].vel.x = ((rand()%100)/50.0f - 1.0f) * 2.0f;
            particles[i].vel.y = ((rand()%100)/50.0f) * 2.0f;
            particles[i].vel.z = ((rand()%100)/50.0f - 1.0f) * 2.0f;
            particles[i].life = 1.0f;
            particles[i].r = r; particles[i].g = g; particles[i].b = b;
            return;
        }
    }
}

void update_render_particles(float dt) {
    glBegin(GL_POINTS);
    for(int i=0; i<MAX_PARTICLES; i++) {
        if(particles[i].life > 0) {
            particles[i].pos.x += particles[i].vel.x * dt;
            particles[i].pos.y += particles[i].vel.y * dt;
            particles[i].pos.z += particles[i].vel.z * dt;
            particles[i].vel.y -= 9.8f * dt; // Gravity
            particles[i].life -= dt * 2.0f;
            
            glColor3f(particles[i].r, particles[i].g, particles[i].b);
            glVertex3f(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z);
        }
    }
    glEnd();
}

#endif

```

### FILE: ./apps/shank-fps/src/net.c
```cpp
#define _WIN32_WINNT 0x0600 // Required for inet_pton
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

int sockfd = -1;
struct sockaddr_in serv_addr;

void net_init(GameContext *game, const char *ip) {
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
#endif
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef _WIN32
    u_long mode = 1; ioctlsocket(sockfd, FIONBIO, &mode);
#else
    int flags = fcntl(sockfd, F_GETFL, 0); fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
#endif
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);
    printf("NET: Socket created. Target: %s:%d\n", ip, PORT);
}

void net_send_input(GameContext *game) {
    if (sockfd < 0) return;
    ClientPacket pkt = {0};
    Player *p = &game->local_player;
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_W]) pkt.fwd += 1.0f;
    if (state[SDL_SCANCODE_S]) pkt.fwd -= 1.0f;
    if (state[SDL_SCANCODE_D]) pkt.strafe += 1.0f;
    if (state[SDL_SCANCODE_A]) pkt.strafe -= 1.0f;
    if (state[SDL_SCANCODE_G]) pkt.throw_grenade = 1;
    if (state[SDL_SCANCODE_E]) pkt.interact = 1;
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) pkt.shoot = 1;
    pkt.yaw = p->yaw; pkt.pitch = p->pitch; pkt.weapon_req = p->current_weapon; 
    pkt.show_scores = state[SDL_SCANCODE_TAB];
    // Cast to const char*
    sendto(sockfd, (const char*)&pkt, sizeof(pkt), 0, (const struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

void net_receive_state(GameContext *game) {
    if (sockfd < 0) return;
    ServerState state;
    struct sockaddr_in from; socklen_t len = sizeof(from);
    int n = recvfrom(sockfd, (char*)&state, sizeof(state), 0, (struct sockaddr*)&from, &len);
    if (n > 0) {
        int my_id = 0; 
        PlayerState *net_me = &state.players[my_id];
        game->local_player.pos = net_me->pos;
        game->local_player.vel = net_me->vel;
        game->local_player.health = net_me->health;
        game->local_player.ammo[net_me->current_weapon] = net_me->ammo[net_me->current_weapon];
        game->last_server_state = state; 
    }
}

```

### FILE: ./apps/shank-fps/src/input.c
```cpp
#include "input.h"
#include <math.h>
#include "../../../packages/protocol/protocol.h" // Needed for WPN constants

#define MOUSE_SENSITIVITY 0.1f
#define MOVE_SPEED 10.0f
#define STRAFE_SPEED 8.0f
#define FRICTION 0.9f

void input_init(GameContext *game) {
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void input_update(GameContext *game, float delta_time) {
    SDL_Event event;
    Player *p = &game->local_player;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) game->running = false;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) game->running = false;
            // FIXED: WPN_MELEE -> WPN_KNIFE
            if (event.key.keysym.sym == SDLK_1) p->current_weapon = WPN_KNIFE;
            if (event.key.keysym.sym == SDLK_2) p->current_weapon = WPN_MAGNUM;
            if (event.key.keysym.sym == SDLK_3) p->current_weapon = WPN_AR;
            if (event.key.keysym.sym == SDLK_4) p->current_weapon = WPN_SHOTGUN;
            if (event.key.keysym.sym == SDLK_5) p->current_weapon = WPN_SNIPER;
        }
        if (event.type == SDL_MOUSEMOTION) {
            p->yaw += event.motion.xrel * MOUSE_SENSITIVITY;
            p->pitch -= event.motion.yrel * MOUSE_SENSITIVITY;
            if (p->pitch > 89.0f) p->pitch = 89.0f;
            if (p->pitch < -89.0f) p->pitch = -89.0f;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    float rad = p->yaw * (M_PI / 180.0f);
    float cos_yaw = cosf(rad);
    float sin_yaw = sinf(rad);

    float fwd_speed = 0.0f;
    float side_speed = 0.0f;

    if (state[SDL_SCANCODE_W]) fwd_speed += MOVE_SPEED;
    if (state[SDL_SCANCODE_S]) fwd_speed -= MOVE_SPEED;
    if (state[SDL_SCANCODE_D]) side_speed += STRAFE_SPEED;
    if (state[SDL_SCANCODE_A]) side_speed -= STRAFE_SPEED;

    p->vel.x += (sin_yaw * fwd_speed + cos_yaw * side_speed) * delta_time;
    p->vel.z -= (cos_yaw * fwd_speed - sin_yaw * side_speed) * delta_time;
    p->pos.x += p->vel.x * delta_time;
    p->pos.z += p->vel.z * delta_time;
    p->vel.x *= FRICTION;
    p->vel.z *= FRICTION;
}

```

### FILE: ./apps/shank-fps/src/font.h
```cpp
#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL_opengl.h>
#include <string.h>
#include <ctype.h>

void draw_char(char c, float x, float y, float s) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    
    // FIX: Flip Y axis (-s) so text draws upright in Top-Left coord system
    glScalef(s, -s, 1.0f); 
    
    glLineWidth(3.0f); 
    glBegin(GL_LINES);
    c = toupper(c);
    
    // (Vector Data)
    if(c=='A'){ glVertex2f(0,0); glVertex2f(0.5,1); glVertex2f(0.5,1); glVertex2f(1,0); glVertex2f(0.2,0.4); glVertex2f(0.8,0.4); }
    if(c=='B'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0.8,0.8); glVertex2f(0.8,0.8); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(0.8,0.2); glVertex2f(0.8,0.2); glVertex2f(0,0); }
    if(c=='C'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); }
    if(c=='D'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0.8,0.5); glVertex2f(0.8,0.5); glVertex2f(0,0); }
    if(c=='E'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); glVertex2f(0,0.5); glVertex2f(0.8,0.5); }
    if(c=='F'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(0,0.5); glVertex2f(0.8,0.5); }
    if(c=='G'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0.5,0.5); }
    if(c=='H'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(1,0); glVertex2f(1,1); glVertex2f(0,0.5); glVertex2f(1,0.5); }
    if(c=='I'){ glVertex2f(0.5,0); glVertex2f(0.5,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(0,0); glVertex2f(1,0); }
    if(c=='J'){ glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(0,0.3); }
    if(c=='K'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(1,1); glVertex2f(0,0.5); glVertex2f(1,0); }
    if(c=='L'){ glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); }
    if(c=='M'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0.5,0.5); glVertex2f(0.5,0.5); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); }
    if(c=='N'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(1,1); }
    if(c=='O'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); }
    if(c=='P'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0,0.5); }
    if(c=='Q'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); glVertex2f(0.5,0.5); glVertex2f(1,0); }
    if(c=='R'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0); }
    if(c=='S'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); }
    if(c=='T'){ glVertex2f(0.5,0); glVertex2f(0.5,1); glVertex2f(0,1); glVertex2f(1,1); }
    if(c=='U'){ glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(1,1); }
    if(c=='V'){ glVertex2f(0,1); glVertex2f(0.5,0); glVertex2f(0.5,0); glVertex2f(1,1); }
    if(c=='W'){ glVertex2f(0,1); glVertex2f(0.2,0); glVertex2f(0.2,0); glVertex2f(0.5,0.5); glVertex2f(0.5,0.5); glVertex2f(0.8,0); glVertex2f(0.8,0); glVertex2f(1,1); }
    if(c=='X'){ glVertex2f(0,0); glVertex2f(1,1); glVertex2f(0,1); glVertex2f(1,0); }
    if(c=='Y'){ glVertex2f(0,1); glVertex2f(0.5,0.5); glVertex2f(1,1); glVertex2f(0.5,0.5); glVertex2f(0.5,0.5); glVertex2f(0.5,0); }
    if(c=='Z'){ glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); }
    if(c=='0'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,1); }
    if(c=='1'){ glVertex2f(0.5,0); glVertex2f(0.5,1); }
    if(c=='2'){ glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); }
    if(c=='3'){ glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); glVertex2f(0,0.5); glVertex2f(1,0.5); }
    if(c=='4'){ glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0.5); glVertex2f(1,1); glVertex2f(1,0); }
    if(c=='5'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); }
    if(c=='6'){ glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0,0.5); }
    if(c=='7'){ glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(0,0); }
    if(c=='8'){ glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0); glVertex2f(1,0); glVertex2f(0,0); glVertex2f(0,0.5); glVertex2f(1,0.5); }
    if(c=='9'){ glVertex2f(1,0); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(1,0.5); }
    if(c==':'){ glVertex2f(0.5,0.8); glVertex2f(0.5,0.7); glVertex2f(0.5,0.3); glVertex2f(0.5,0.2); }
    if(c=='-'){ glVertex2f(0,0.5); glVertex2f(1,0.5); }
    if(c=='>'){ glVertex2f(0,1); glVertex2f(0.5,0.5); glVertex2f(0.5,0.5); glVertex2f(0,0); }
    if(c=='<'){ glVertex2f(0.5,1); glVertex2f(0,0.5); glVertex2f(0,0.5); glVertex2f(0.5,0); }
    if(c=='['){ glVertex2f(0.5,1); glVertex2f(0,1); glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0,0); glVertex2f(0.5,0); }
    if(c==']'){ glVertex2f(0,1); glVertex2f(0.5,1); glVertex2f(0.5,1); glVertex2f(0.5,0); glVertex2f(0.5,0); glVertex2f(0,0); }
    if(c=='_'){ glVertex2f(0,0); glVertex2f(1,0); }
    if(c=='?'){ glVertex2f(0,1); glVertex2f(1,1); glVertex2f(1,1); glVertex2f(1,0.5); glVertex2f(1,0.5); glVertex2f(0.5,0.5); glVertex2f(0.5,0.5); glVertex2f(0.5,0.2); }

    glEnd();
    glPopMatrix();
}

void draw_text(const char* str, float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    float cx = x;
    while(*str) {
        draw_char(*str, cx, y, size);
        cx += size * 1.5f; 
        str++;
    }
}
#endif

```

### FILE: ./apps/shank-fps/src/main.c
```cpp
#define SDL_MAIN_HANDLED
#include <stdio.h>
#include "shank.h"
#include "input.h"
#include "net.h"
#include "font.h"
#include "particles.h"
#include <GL/glu.h>
#include "../../../packages/map/map.h"
#include "input.c"
#include "net.c"
#include "../../../packages/map/map.c"

GameContext game;
GameMap level;
int show_scoreboard = 0;
int show_help = 0;
char kill_feed[5][64];
int kill_timer = 0;

void add_kill_msg(const char* msg) {
    for(int i=0; i<4; i++) strcpy(kill_feed[i], kill_feed[i+1]);
    strcpy(kill_feed[4], msg); kill_timer = 300;
}

void init_opengl() {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(FOV, (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW); glPointSize(4.0f);
}

void render_hud() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

    Player *p = &game.local_player;
    char buf[64];
    
    // HP/Ammo
    sprintf(buf, "HP %d", p->health);
    draw_text(buf, 50, WINDOW_HEIGHT - 80, 40.0f, 1, p->health < 30 ? 0 : 1, 0.2);
    sprintf(buf, "AMMO %d", p->ammo[p->current_weapon]);
    draw_text(buf, WINDOW_WIDTH - 300, WINDOW_HEIGHT - 80, 40.0f, 1, 1, 0);

    // Timer
    int seconds = (game.last_server_state.match_timer / 60) % 60;
    int minutes = (game.last_server_state.match_timer / 60) / 60;
    sprintf(buf, "%02d:%02d", minutes, seconds);
    draw_text(buf, WINDOW_WIDTH/2 - 80, 50, 40.0f, 1, 1, 1);

    // Help Hint
    draw_text("[H] HELP", 20, 20, 15.0f, 0.5, 0.5, 0.5);

    // Toggle Help Overlay
    if (show_help) {
        glColor4f(0,0,0,0.8f);
        glBegin(GL_QUADS); glVertex2f(200, 150); glVertex2f(WINDOW_WIDTH-200, 150); glVertex2f(WINDOW_WIDTH-200, 450); glVertex2f(200, 450); glEnd();
        draw_text("COMMANDS", WINDOW_WIDTH/2 - 100, 180, 30.0f, 1, 1, 0);
        draw_text("WASD : MOVE", 250, 240, 20.0f, 1,1,1);
        draw_text("MOUSE : AIM / SHOOT", 250, 270, 20.0f, 1,1,1);
        draw_text("1 - 5 : WEAPONS", 250, 300, 20.0f, 1,1,1);
        draw_text("G : GRENADE", 250, 330, 20.0f, 1,1,1);
        draw_text("E : ENTER VEHICLE", 250, 360, 20.0f, 1,1,1);
        draw_text("TAB : SCOREBOARD", 250, 390, 20.0f, 1,1,1);
    }

    if (kill_timer > 0) {
        kill_timer--;
        for(int i=0; i<5; i++) { if(strlen(kill_feed[i]) > 0) draw_text(kill_feed[i], WINDOW_WIDTH - 400, 100 + (i*40), 20.0f, 1, 0.5, 0); }
    }

    glBegin(GL_LINES); glColor3f(0,1,1);
    glVertex2f(WINDOW_WIDTH/2 - 15, WINDOW_HEIGHT/2); glVertex2f(WINDOW_WIDTH/2 + 15, WINDOW_HEIGHT/2);
    glVertex2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 15); glVertex2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 15);
    glEnd();

    if (show_scoreboard) {
        glColor4f(0,0,0,0.8f);
        glBegin(GL_QUADS); glVertex2f(200, 100); glVertex2f(WINDOW_WIDTH-200, 100); glVertex2f(WINDOW_WIDTH-200, WINDOW_HEIGHT-100); glVertex2f(200, WINDOW_HEIGHT-100); glEnd();
        draw_text("SCOREBOARD", WINDOW_WIDTH/2 - 150, 120, 30.0f, 1,1,1);
        for(int i=0; i<MAX_CLIENTS; i++) {
            PlayerState *s = &game.last_server_state.players[i];
            if(s->active) {
                sprintf(buf, "PLAYER %d   K: %d   D: %d", i, s->kills, s->deaths);
                draw_text(buf, 250, 200 + (i*50), 25.0f, 0.8, 0.8, 0.8);
            }
        }
    }

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}

void render() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(-game.local_player.pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(-game.local_player.yaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(-game.local_player.pos.x, -game.local_player.pos.y - 1.7f, -game.local_player.pos.z);

    for(int i=0; i<level.wall_count; i++) {
        Wall *w = &level.walls[i]; glPushMatrix(); glTranslatef(w->x, w->y, w->z); glScalef(w->sx, w->sy, w->sz);
        glColor3f(w->r, w->g, w->b);
        glBegin(GL_QUADS);
            glVertex3f(-1,-1,1); glVertex3f(1,-1,1); glVertex3f(1,1,1); glVertex3f(-1,1,1);
            glVertex3f(-1,-1,-1); glVertex3f(-1,1,-1); glVertex3f(1,1,-1); glVertex3f(1,-1,-1);
            glVertex3f(-1,-1,-1); glVertex3f(-1,-1,1); glVertex3f(-1,1,1); glVertex3f(-1,1,-1);
            glVertex3f(1,-1,-1); glVertex3f(1,1,-1); glVertex3f(1,1,1); glVertex3f(1,-1,1);
            glVertex3f(-1,1,-1); glVertex3f(-1,1,1); glVertex3f(1,1,1); glVertex3f(1,1,-1);
            glVertex3f(-1,-1,-1); glVertex3f(1,-1,-1); glVertex3f(1,-1,1); glVertex3f(-1,-1,1);
        glEnd(); glPopMatrix();
    }
    
    glBegin(GL_LINES); glColor3f(0.3f, 0.3f, 0.3f); for(int i=-50; i<=50; i+=2) { glVertex3f(i,0,-50); glVertex3f(i,0,50); glVertex3f(-50,0,i); glVertex3f(50,0,i); } glEnd();
    update_render_particles(0.016f);

    for(int i=0; i<MAX_CLIENTS; i++) {
        PlayerState *p = &game.last_server_state.players[i];
        if (!p->active || i == 0 || p->vehicle_id != -1) continue; 
        if (p->hit_feedback == 1) spawn_particle(p->pos, 0, 0, 1);
        if (p->hit_feedback == 2) spawn_particle(p->pos, 1, 0, 0);
        glPushMatrix(); glTranslatef(p->pos.x, p->pos.y, p->pos.z);
        glColor3f(1.0f, 0.2f, 0.2f); if (p->is_shooting > 0) glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS); glVertex3f(-0.5f, 0.0f, 0.5f); glVertex3f(0.5f, 0.0f, 0.5f); glVertex3f(0.5f, 1.8f, 0.5f); glVertex3f(-0.5f, 1.8f, 0.5f); glEnd();
        glPopMatrix();
    }
    render_hud(); SDL_GL_SwapWindow(game.window);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    game.window = SDL_CreateWindow("SHANK PIT ONLINE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    game.gl_context = SDL_GL_CreateContext(game.window);
    init_opengl(); input_init(&game); net_init(&game, "127.0.0.1"); map_init(&level); particles_init();
    game.running = true; Uint32 last_time = SDL_GetTicks();
    while (game.running) {
        Uint32 current_time = SDL_GetTicks(); float delta_time = (current_time - last_time) / 1000.0f; last_time = current_time;
        
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) game.running = 0;
            if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_ESCAPE) game.running = 0;
                if(event.key.keysym.sym == SDLK_h) show_help = !show_help;
            }
        }

        const Uint8 *state = SDL_GetKeyboardState(NULL); show_scoreboard = state[SDL_SCANCODE_TAB];
        input_update(&game, delta_time); net_send_input(&game); net_receive_state(&game); render();
    }
    SDL_GL_DeleteContext(game.gl_context); SDL_DestroyWindow(game.window); SDL_Quit();
    return 0;
}

```

### FILE: ./apps/fishing/src/fishing.c
```cpp
#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include "font.h" // Shared ShankEngine Font

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("SKATECHAIN // FISHING v1.1", 100, 100, 800, 600, 0);
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    float bar_pos = 0.5f;
    float fish_pos = 0.5f;
    float progress = 0.3f;
    int is_reeling = 0;
    char status_msg[64] = "READY TO CAST";

    int running = 1;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) is_reeling = 1;
            if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_SPACE) is_reeling = 0;
        }

        if(is_reeling) bar_pos -= 0.015f; else bar_pos += 0.005f;
        if(bar_pos < 0.1f) bar_pos = 0.1f; if(bar_pos > 0.9f) bar_pos = 0.9f;

        fish_pos += sinf(SDL_GetTicks() * 0.005f) * 0.02f;

        if(fabs(fish_pos - bar_pos) < 0.1f) progress += 0.002f;
        else progress -= 0.001f;

        if(progress >= 1.0f) {
            sprintf(status_msg, "VERIFYING CATCH ON-CHAIN...");
            progress = 0.3f; 
        }

        SDL_SetRenderDrawColor(rend, 10, 20, 40, 255);
        SDL_RenderClear(rend);

        // UI Overlay via Shared Engine
        // Note: shank_draw_text is a placeholder for the actual render loop
        // In the binary, this will render the pixel font we built.

        SDL_SetRenderDrawColor(rend, 0, 255, 150, 255);
        SDL_Rect r_bar = {700, (int)(bar_pos * 600), 15, 70};
        SDL_RenderFillRect(rend, &r_bar);

        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_Rect r_fish = {702, (int)(fish_pos * 600), 10, 10};
        SDL_RenderFillRect(rend, &r_fish);

        SDL_RenderPresent(rend);
        SDL_Delay(16);
    }
    SDL_Quit();
    return 0;
}

```

### FILE: ./apps/lobby/src/main.c
```cpp
#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <string.h>

#define WPN_KNIFE 0
#define WPN_MAGNUM 1
#define WPN_AR 2
#define WPN_SHOTGUN 3
#define WPN_SNIPER 4

int current_weapon = WPN_MAGNUM;

void draw_weapon_model() {
    glPushMatrix();
    glTranslatef(0.5f, -0.5f, -1.0f);
    switch(current_weapon) {
        case WPN_KNIFE:   glColor3f(0.8f, 0.8f, 0.8f); glScalef(0.1f, 0.1f, 0.4f); break;
        case WPN_MAGNUM:  glColor3f(0.3f, 0.3f, 0.3f); glScalef(0.15f, 0.2f, 0.3f); break;
        case WPN_AR:      glColor3f(0.2f, 0.4f, 0.2f); glScalef(0.15f, 0.2f, 0.8f); break;
        case WPN_SHOTGUN: glColor3f(0.4f, 0.3f, 0.2f); glScalef(0.2f, 0.25f, 0.6f); break;
        case WPN_SNIPER:  glColor3f(0.1f, 0.1f, 0.1f); glScalef(0.1f, 0.15f, 1.2f); break;
    }
    // Draw simple box for weapon
    glBegin(GL_QUADS);
    glVertex3f(-1,-1,1); glVertex3f(1,-1,1); glVertex3f(1,1,1); glVertex3f(-1,1,1);
    glVertex3f(-1,-1,-1); glVertex3f(-1,1,-1); glVertex3f(1,1,-1); glVertex3f(1,-1,-1);
    glEnd();
    glPopMatrix();
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("SHANKPIT // INFANTRY", 100, 100, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(win);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(70.0, 1280.0/720.0, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    int running = 1;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
            if(e.type == SDL_KEYDOWN) {
                if(e.key.keysym.sym == SDLK_1) current_weapon = WPN_KNIFE;
                if(e.key.keysym.sym == SDLK_2) current_weapon = WPN_MAGNUM;
                if(e.key.keysym.sym == SDLK_3) current_weapon = WPN_AR;
                if(e.key.keysym.sym == SDLK_4) current_weapon = WPN_SHOTGUN;
                if(e.key.keysym.sym == SDLK_5) current_weapon = WPN_SNIPER;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        
        draw_weapon_model();

        SDL_GL_SwapWindow(win);
        SDL_Delay(16);
    }
    SDL_Quit();
    return 0;
}

```

### FILE: ./apps/server/src/main.c
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6969
#define MTU_SIZE 1400

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    char buffer[MTU_SIZE];

    printf("🔫 ShankPit Infantry Server v95 (MTU Fixed) Starting...\n");

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed"); exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed"); exit(EXIT_FAILURE);
    }

    printf("📡 Listening on port %d...\n", PORT);
    
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while(1) {
        int n = recvfrom(server_fd, buffer, MTU_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (n > 0) {
            // Process Infantry Input
            // Echo back state (simplified for restoration)
            sendto(server_fd, buffer, n, 0, (struct sockaddr*)&client_addr, addr_len);
        }
    }
    return 0;
}

```

### FILE: ./apps/bot_client/swarm_launcher.py
```cpp

import subprocess
import time
import os
import sys
import select

PROJECT_ROOT = "/content/drive/MyDrive/skateboard"
BOT_RUNNER = os.path.join(PROJECT_ROOT, "apps", "bot_client", "bot_runner.py")

DEFAULT_HOST = "s.farthq.com"
DEFAULT_PORT = 5000

def launch_swarm(host, port, instances=6):
    print(f"🚀 LAUNCHING VERBOSE SWARM ({instances} Clients)...")
    print(f"   Target: {host}:{port}")
    
    procs = []
    
    for i in range(instances):
        # We start each process and PIPE stdout so we can read it
        cmd = ["python", "-u", BOT_RUNNER, "--host", host, "--port", str(port), "--bots", "1"]
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1)
        procs.append(p)
    
    print("✅ SWARM ACTIVE. LISTENING TO COMMS (Ctrl+C to Stop)...\n")

    try:
        while True:
            # Poll stdout of all bots
            reads = [p.stdout.fileno() for p in procs]
            ret = select.select(reads, [], [], 0.1)

            for fd in ret[0]:
                for i, p in enumerate(procs):
                    if p.stdout.fileno() == fd:
                        line = p.stdout.readline()
                        if line:
                            # Filter boring lines if needed
                            clean = line.strip()
                            if "HP:" in clean or "CONNECTION" in clean or "SILENCE" in clean:
                                print(f"[Inst {i}] {clean}")
    except KeyboardInterrupt:
        print("\n🛑 STOPPING SWARM...")
    finally:
        for p in procs: p.terminate()

if __name__ == "__main__":
    launch_swarm(DEFAULT_HOST, DEFAULT_PORT)

```

### FILE: ./apps/bot_client/bot_runner.py
```cpp

import argparse
import socket
import struct
import time
import os
import subprocess
import torch
import torch.nn as nn
from torch.distributions import Categorical
import numpy as np
import sys

PROJECT_ROOT = "/content/drive/MyDrive/skateboard"
MODEL_PATH = os.path.join(PROJECT_ROOT, "shank_ai.pth")

# --- PROTOCOL v3 (10 Players) ---
SERVER_STATE_FMT = f"i {10 * 'i 3f 3f f f i i i 5i i i i i i'}" 
INPUT_FMT = "f f f f i i i i"

class ShankBrain(nn.Module):
    def __init__(self):
        super(ShankBrain, self).__init__()
        self.fc1 = nn.Linear(8, 128)
        self.fc2 = nn.Linear(128, 64)
        self.actor_move = nn.Linear(64, 4)   
        self.actor_aim = nn.Linear(64, 2)    
        self.actor_shoot = nn.Linear(64, 2)  
        self.critic = nn.Linear(64, 1)       

    def forward(self, x):
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        move_probs = torch.softmax(self.actor_move(x), dim=-1)
        shoot_probs = torch.softmax(self.actor_shoot(x), dim=-1)
        aim_mean = torch.tanh(self.actor_aim(x))
        return move_probs, aim_mean, shoot_probs, None

class BotClient:
    def __init__(self, host, port, bot_id, brain):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setblocking(False)
        self.addr = (host, port)
        self.id = bot_id
        self.brain = brain
        self.my_state = None
        self.yaw = 0.0
        self.pitch = 0.0
        self.last_packet_time = 0
        self.connected = False

    def think(self):
        try:
            data, _ = self.sock.recvfrom(4096)
            self.last_packet_time = time.time()
            if not self.connected:
                print(f"[Bot {self.id}] 🟢 CONNECTED (10-Player Lobby)")
                self.connected = True
            obs = self.parse_state(data)
        except BlockingIOError:
            obs = None
            if self.connected and (time.time() - self.last_packet_time > 3.0):
                 self.connected = False

        if obs is not None:
            with torch.no_grad():
                move_probs, aim_mean, shoot_probs, _ = self.brain(obs)
            m = Categorical(move_probs).sample().item()
            s = Categorical(shoot_probs).sample().item()
            self.send_input((m, aim_mean[0].item(), aim_mean[1].item(), s))
        else:
            self.send_input((0,0,0,0))

    def parse_state(self, data):
        try:
            sz = struct.calcsize(SERVER_STATE_FMT)
            if len(data) < sz: return None
            unpacked = struct.unpack(SERVER_STATE_FMT, data[:sz])
            
            p_size = 23
            target_slot = (self.id + 2) % 10 # Update modulo for 10
            
            enemies = []

            for i in range(10): # Loop 10
                base = 1 + (i * p_size)
                p_data = unpacked[base : base + p_size]
                if p_data[0] == 0: continue 
                
                p_dict = {'pos': np.array(p_data[1:4]), 'hp': p_data[9], 'kills': p_data[10], 'ammo': p_data[13]}
                
                if i == target_slot: self.my_state = p_dict
                else: enemies.append(p_dict)

            if self.my_state is None: return None

            closest_dist = 9999.0
            rel_pos = np.array([0.0, 0.0, 0.0])
            enemy_hp = 0
            
            for e in enemies:
                dist = np.linalg.norm(e['pos'] - self.my_state['pos'])
                if dist < closest_dist:
                    closest_dist = dist
                    rel_pos = e['pos'] - self.my_state['pos']
                    enemy_hp = e['hp']
            
            return torch.tensor([
                self.my_state['hp'] / 100.0,
                self.my_state['ammo'] / 6.0,
                rel_pos[0] / 50.0,
                rel_pos[1] / 10.0,
                rel_pos[2] / 50.0,
                closest_dist / 50.0,
                enemy_hp / 100.0,
                1.0
            ], dtype=torch.float32)
        except Exception: return None

    def send_input(self, action):
        m, yd, pd, s = action
        fwd=0.0; strf=0.0
        if m==0: fwd=1.0
        elif m==1: fwd=-1.0
        elif m==2: strf=-1.0
        elif m==3: strf=1.0
        self.yaw += yd * 5.0
        self.pitch += pd * 5.0
        reload = 1 if self.my_state and self.my_state['ammo'] == 0 else 0
        header = struct.pack("i", 1)
        payload = struct.pack(INPUT_FMT, fwd, strf, self.yaw, self.pitch, 0, 0, s, reload)
        self.sock.sendto(header + payload, self.addr)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--host', default="127.0.0.1")
    parser.add_argument('--port', type=int, default=5000)
    parser.add_argument('--bots', type=int, default=1)
    args = parser.parse_known_args()[0]

    brain = ShankBrain()
    if os.path.exists(MODEL_PATH): brain.load_state_dict(torch.load(MODEL_PATH))
    
    target_host = socket.gethostbyname(args.host)
    bots = [BotClient(target_host, args.port, i, brain) for i in range(args.bots)]

    try:
        while True:
            for bot in bots: bot.think()
            time.sleep(0.016)
    except KeyboardInterrupt: pass

```

### FILE: ./apps/sitegen/main.h
```cpp
#ifndef SITEGEN_H
#define SITEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Config Data
typedef struct {
    char title[128];
    char tagline[256];
    char hero_text[512];
    char cta_text[128];
    char cta_link[128];
    char bg_color[32];
    char accent_color[32];
    char text_color[32];
} SiteConfig;

// Functions
int load_config(const char* filename, SiteConfig* config);
int generate_site(const SiteConfig* config);
void serve_site(int port, const char* web_root);

#endif

```

### FILE: ./apps/sitegen/content.json
```cpp
{
  "title": "SHANKPIT ENGINE",
  "tagline": "FRAGMENT THE VOID",
  "hero_text": "A minimal, hackable FPS engine built in C. No bloat. Pure velocity. 4WD Physics. Neon Aesthetics. Join the live development cycle.",
  "cta_text": "VIEW SOURCE CODE",
  "cta_link": "https://gitlab.com/danowski/skateboard",
  "bg_color": "#050505",
  "accent_color": "#00ffea"
}

```

### FILE: ./apps/sitegen/main.c
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "main.h"

// --- JSON PARSER ---
void parse_val(const char* json, const char* key, char* dest, int max) {
    char search[128]; sprintf(search, "\"%s\"", key);
    char* found = strstr(json, search);
    if (!found) { strcpy(dest, "MISSING"); return; }
    char* start = strchr(found, ':'); if (!start) return;
    start = strchr(start, '"'); if (!start) return; start++;
    int i=0; while(start[i]!='"' && i<max-1) { dest[i]=start[i]; i++; } dest[i]=0;
}

int load_config(const char* filename, SiteConfig* config) {
    FILE* f = fopen(filename, "r");
    if (!f) return 0;
    char buf[4096]; size_t len = fread(buf, 1, 4096, f); buf[len]=0; fclose(f);
    parse_val(buf, "title", config->title, 128);
    parse_val(buf, "tagline", config->tagline, 256);
    parse_val(buf, "hero_text", config->hero_text, 512);
    parse_val(buf, "cta_text", config->cta_text, 128);
    parse_val(buf, "cta_link", config->cta_link, 128);
    parse_val(buf, "bg_color", config->bg_color, 32);
    parse_val(buf, "accent_color", config->accent_color, 32);
    strcpy(config->text_color, "#ffffff");
    return 1;
}

// --- GENERATOR ---
int generate_site(const SiteConfig* config) {
    // HTML
    FILE* f = fopen("output/index.html", "w"); if (!f) return 0;
    fprintf(f, "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>%s</title><link rel='stylesheet' href='style.css'></head><body>", config->title);
    fprintf(f, "<div class='container'><header><div class='logo'>%s</div></header>", config->title);
    fprintf(f, "<main class='hero'><h1>%s</h1><p class='subtext'>%s</p><a href='%s' class='cta-button'>%s</a></main>", config->tagline, config->hero_text, config->cta_link, config->cta_text);
    fprintf(f, "<footer><p>POWERED BY SKATECHAIN ENGINE</p></footer></div></body></html>");
    fclose(f);

    // CSS
    f = fopen("output/style.css", "w"); if (!f) return 0;
    fprintf(f, "body{margin:0;padding:0;background-color:%s;color:%s;font-family:monospace;text-align:center;}", config->bg_color, config->text_color);
    fprintf(f, ".container{max-width:800px;margin:0 auto;padding:2rem;} header{margin-bottom:4rem;text-align:left;}");
    fprintf(f, ".logo{font-weight:bold;font-size:1.5rem;color:%s;letter-spacing:2px;}", config->accent_color);
    fprintf(f, ".hero{margin-top:15vh;} h1{font-size:3.5rem;margin-bottom:1rem;line-height:1;} .subtext{font-size:1.2rem;opacity:0.8;margin-bottom:3rem;}");
    fprintf(f, ".cta-button{display:inline-block;color:%s;border:2px solid %s;padding:15px 30px;font-size:1.2rem;text-decoration:none;font-weight:bold;transition:0.2s;}", config->accent_color, config->accent_color);
    fprintf(f, ".cta-button:hover{background-color:%s;color:%s;box-shadow:0 0 20px %s;} footer{margin-top:5rem;font-size:0.8rem;opacity:0.3;}", config->accent_color, config->bg_color, config->accent_color);
    fclose(f);
    return 1;
}

// --- TINY HTTP SERVER ---
void send_response(int client_fd, const char* status, const char* type, const char* body, int len) {
    char header[1024];
    sprintf(header, "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", status, type, len);
    send(client_fd, header, strlen(header), 0);
    send(client_fd, body, len, 0);
}

void serve_file(int client_fd, const char* path, const char* type) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        char* msg = "404 Not Found";
        send_response(client_fd, "404 Not Found", "text/plain", msg, strlen(msg));
        return;
    }
    fseek(f, 0, SEEK_END); long size = ftell(f); fseek(f, 0, SEEK_SET);
    char* buf = malloc(size);
    fread(buf, 1, size, f);
    fclose(f);
    send_response(client_fd, "200 OK", type, buf, size);
    free(buf);
}

void serve_site(int port, const char* web_root) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    int opt = 1; setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed"); exit(1);
    }
    listen(server_fd, 10);
    printf("📡 HTTP Server listening on port %d (Root: %s)\n", port, web_root);

    while(1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;
        
        char buf[2048];
        int len = recv(client_fd, buf, 2047, 0);
        if (len > 0) {
            buf[len] = 0;
            char method[16], url[256];
            sscanf(buf, "%s %s", method, url);
            
            if (strcmp(method, "GET") == 0) {
                char filepath[512];
                if (strcmp(url, "/") == 0) {
                    sprintf(filepath, "%s/index.html", web_root);
                    serve_file(client_fd, filepath, "text/html");
                } else if (strcmp(url, "/style.css") == 0) {
                    sprintf(filepath, "%s/style.css", web_root);
                    serve_file(client_fd, filepath, "text/css");
                } else {
                    char* msg = "404";
                    send_response(client_fd, "404 Not Found", "text/plain", msg, 3);
                }
            }
        }
        close(client_fd);
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "serve") == 0) {
        // SERVER MODE
        int port = 80;
        char* dir = "output";
        if (argc > 2) port = atoi(argv[2]);
        if (argc > 3) dir = argv[3];
        serve_site(port, dir);
    } else {
        // GENERATOR MODE
        SiteConfig config;
        printf("🔨 CSSG v2: Generating Site...\n");
        if (!load_config("content.json", &config)) {
            printf("❌ Missing content.json\n"); return 1;
        }
        if (generate_site(&config) && generate_css(&config)) {
            printf("✅ Site Generated in /output\n");
            printf("👉 Run './sitegen serve 8080' to test\n");
        }
    }
    return 0;
}

```

### FILE: ./apps/empire/empire.json
```cpp
{
  "meta": {
    "empire_name": "Iron Syndicate",
    "owner": "Emiree",
    "last_updated": "2026-01-24 12:00"
  },
  "economy": {
    "gold": 15400000,
    "hourly_rate": 250000
  },
  "agents": [
    {"name":"Miner_Alpha","task":"mining_coal","xp":35000,"status":"active","x":10,"z":5},
    {"name":"Market_Bot_7","task":"flipping","xp":0,"status":"idle","x":-5,"z":-5},
    {"name":"Slayer_01","task":"combat","xp":120000,"status":"active","x":2,"z":8},
    {"name":"Woodcutter_X","task":"chopping","xp":4500,"status":"active","x":-8,"z":8}
  ]
}

```

### FILE: ./apps/empire/main.h
```cpp
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

```

### FILE: ./apps/empire/font.h
```cpp
#ifndef EMPIRE_FONT_H
#define EMPIRE_FONT_H

#include <GL/gl.h>
#include <string.h>

// A simple 5x7 bitmapped font (0-9, A-Z, symbols)
// Compressed for brevity. In a real engine we'd load a texture.
// For this tool, we render GL_POINTS or GL_LINES for "Hacker" aesthetic.

void draw_char(char c, float x, float y, float size) {
    // Simplified Vector Font (Just a placeholder for visual clarity)
    // Real implementation would parse a bitmap.
    // Here we just draw a box representation for text flow testing.
    
    // NOTE: For a proper build without assets, we use GLUT or system fonts usually.
    // Since we are pure SDL/GL, we will render a colored quad for the cursor/text pos
    // to verify layout, then user can drop in a real font texture later.
    
    // Placeholder: Draw a small quad
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size*0.6f, y);
    glVertex2f(x + size*0.6f, y + size);
    glVertex2f(x, y + size);
    glEnd();
}

void draw_text(const char* text, float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    float cursor_x = x;
    
    // Since we don't have the asset loaded in this recovery script,
    // we will rely on a "Debug Text" mode or just logging for now.
    // BUT to fix the compile error, this function signature must exist.
    
    // In the future: Load a texture.
    // For now: We compile successfully.
    
    // (Optional: Draw dots to represent characters so we see length)
    for(size_t i=0; i<strlen(text); i++) {
        draw_char(text[i], cursor_x, y, size);
        cursor_x += size * 0.8f;
    }
}

#endif

```

### FILE: ./apps/empire/main.c
```cpp
#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

// --- SHARED LOGIC ---
void parse_str(const char* json, const char* key, char* dest) {
    char search[64]; sprintf(search, "\"%s\":", key);
    char* found = strstr(json, search);
    if(!found) return;
    char* start = strchr(found, ':'); if(!start) return;
    start = strchr(start, '"'); if(!start) return; start++;
    int i=0; while(start[i]!='"' && i<31) { dest[i]=start[i]; i++; } dest[i]=0;
}

long parse_int(const char* json, const char* key) {
    char search[64]; sprintf(search, "\"%s\":", key);
    char* found = strstr(json, search);
    if(!found) return 0;
    return strtol(strchr(found, ':')+1, NULL, 10);
}

void empire_load(const char* filename, EmpireState* state) {
    FILE* f = fopen(filename, "r");
    if(!f) return;
    char buf[8192]; size_t len = fread(buf, 1, 8192, f); buf[len]=0; fclose(f);
    
    parse_str(buf, "empire_name", state->empire_name);
    state->gold = parse_int(buf, "gold");
    state->hourly_rate = parse_int(buf, "hourly_rate");
    
    state->agent_count = 0;
    char* cursor = buf;
    while( (cursor = strstr(cursor, "name")) && state->agent_count < 64 ) {
        char* obj_start = cursor;
        while(*obj_start != '{' && obj_start > buf) obj_start--;
        Agent* a = &state->agents[state->agent_count++];
        parse_str(obj_start, "name", a->name);
        parse_str(obj_start, "task", a->task);
        parse_str(obj_start, "status", a->status);
        a->xp = parse_int(obj_start, "xp");
        a->x = (float)parse_int(obj_start, "x");
        a->z = (float)parse_int(obj_start, "z");
        cursor += 10;
    }
}

void empire_generate_site(EmpireState* state) {
    FILE* f = fopen("output/index.html", "w");
    if(!f) return;
    fprintf(f, "<html><body style='background:#111;color:#0f0;font-family:monospace'><h1>%s</h1>", state->empire_name);
    fprintf(f, "<p>GOLD: %ld | XP/HR: %ld</p></body></html>", state->gold, state->hourly_rate);
    fclose(f);
}

#ifdef _WIN32
// --- VISUALIZER MODE ---
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include "font.h" // FIXED: Local Include

EmpireState state;
float cam_x=0, cam_y=15, cam_z=20;
float cam_yaw=0, cam_pitch=-30;
int console_open = 0;
char cmd_buf[256] = "";
char log_buf[5][128];

void log_msg(const char* msg) {
    for(int i=4; i>0; i--) strcpy(log_buf[i], log_buf[i-1]);
    strncpy(log_buf[0], msg, 127);
}

void process_command() {
    if (strlen(cmd_buf) == 0) return;
    if (strcmp(cmd_buf, "/help") == 0) {
        log_msg("--- COMMANDS ---");
        log_msg("/status  - Show economy");
        log_msg("/reload  - Reload JSON");
        log_msg("/quit    - Exit");
    }
    else if (strcmp(cmd_buf, "/reload") == 0) {
        empire_load("empire.json", &state);
        log_msg("Reloaded empire.json");
    }
    else if (strcmp(cmd_buf, "/status") == 0) {
        char buf[128]; sprintf(buf, "GOLD: %ld | RATE: %ld/hr", state.gold, state.hourly_rate);
        log_msg(buf);
    }
    else if (strcmp(cmd_buf, "/quit") == 0) exit(0);
    else log_msg("Unknown command.");
    cmd_buf[0] = 0;
}

void draw_cube(float x, float y, float z, float r, float g, float b) {
    glPushMatrix(); glTranslatef(x,y,z); glColor3f(r,g,b);
    glBegin(GL_QUADS);
    glVertex3f(-0.5,0.5,0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,0.5);
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,-0.5,-0.5); glVertex3f(-0.5,-0.5,-0.5);
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(-0.5,0.5,0.5);
    glVertex3f(-0.5,-0.5,-0.5); glVertex3f(0.5,-0.5,-0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,0.5);
    glVertex3f(-0.5,0.5,-0.5); glVertex3f(-0.5,0.5,0.5); glVertex3f(-0.5,-0.5,0.5); glVertex3f(-0.5,-0.5,-0.5);
    glVertex3f(0.5,0.5,-0.5); glVertex3f(0.5,0.5,0.5); glVertex3f(0.5,-0.5,0.5); glVertex3f(0.5,-0.5,-0.5);
    glEnd(); glPopMatrix();
}

int main(int argc, char* argv[]) {
    empire_load("empire.json", &state);
    log_msg("Welcome to Empire Command.");
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("EMPIRE COMMAND v2", 100, 100, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext gl = SDL_GL_CreateContext(win);
    
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(60.0f, 1280.0f/720.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW); glEnable(GL_DEPTH_TEST);

    int running = 1;
    SDL_StartTextInput();
    
    while(running) {
        SDL_Event e; 
        while(SDL_PollEvent(&e)) {
            if(e.type==SDL_QUIT) running=0;
            if (console_open) {
                if(e.type == SDL_KEYDOWN) {
                    if(e.key.keysym.sym == SDLK_RETURN) { process_command(); console_open = 0; }
                    if(e.key.keysym.sym == SDLK_ESCAPE) { console_open = 0; }
                    if(e.key.keysym.sym == SDLK_BACKSPACE && strlen(cmd_buf) > 0) cmd_buf[strlen(cmd_buf)-1] = 0;
                }
                if(e.type == SDL_TEXTINPUT) strcat(cmd_buf, e.text.text);
            } else {
                if(e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_SLASH || e.key.keysym.sym == SDLK_RETURN)) { 
                    console_open = 1; if(e.key.keysym.sym == SDLK_SLASH) strcpy(cmd_buf, "/"); else cmd_buf[0] = 0; 
                }
                if (e.type == SDL_MOUSEMOTION && (SDL_GetMouseState(NULL,NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))) {
                    cam_yaw -= e.motion.xrel * 0.2f; cam_pitch -= e.motion.yrel * 0.2f;
                }
            }
        }
        
        if (!console_open) {
            const Uint8* k = SDL_GetKeyboardState(NULL);
            float r = -cam_yaw * 0.01745f;
            float fwd_x = sinf(r), fwd_z = -cosf(r), right_x = cosf(r), right_z = sinf(r);
            if(k[SDL_SCANCODE_W]) { cam_x += fwd_x*0.5f; cam_z += fwd_z*0.5f; }
            if(k[SDL_SCANCODE_S]) { cam_x -= fwd_x*0.5f; cam_z -= fwd_z*0.5f; }
            if(k[SDL_SCANCODE_D]) { cam_x += right_x*0.5f; cam_z += right_z*0.5f; }
            if(k[SDL_SCANCODE_A]) { cam_x -= right_x*0.5f; cam_z -= right_z*0.5f; }
            if(k[SDL_SCANCODE_SPACE]) cam_y += 0.5f;
            if(k[SDL_SCANCODE_LSHIFT]) cam_y -= 0.5f;
        }

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity(); glRotatef(-cam_pitch, 1, 0, 0); glRotatef(-cam_yaw, 0, 1, 0); glTranslatef(-cam_x, -cam_y, -cam_z);
        
        glBegin(GL_LINES); glColor3f(0.2f, 0.2f, 0.2f);
        for(int i=-50; i<=50; i+=5) { glVertex3f(i,0,-50); glVertex3f(i,0,50); glVertex3f(-50,0,i); glVertex3f(50,0,i); } glEnd();
        
        for(int i=0; i<state.agent_count; i++) {
            Agent* a = &state.agents[i];
            float r = (strcmp(a->status, "active")==0) ? 0.0f : 1.0f; 
            float g = (strcmp(a->status, "active")==0) ? 1.0f : 0.0f; 
            draw_cube(a->x, 0.5f, a->z, r, g, 0.0f);
        }
        
        glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D(0, 1280, 720, 0);
        glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); glDisable(GL_DEPTH_TEST);
        
        draw_text(state.empire_name, 20, 20, 30.0f, 1, 1, 1);
        for(int i=0; i<5; i++) draw_text(log_buf[i], 20, 600 - (i*25), 18.0f, 0.7f, 0.7f, 0.7f);
        
        if (console_open) { char buf[300]; sprintf(buf, "> %s_", cmd_buf); draw_text(buf, 20, 680, 20.0f, 0, 1, 0); } 
        else draw_text("[ / ] to open console", 20, 680, 18.0f, 0.5f, 0.5f, 0.5f);

        glEnable(GL_DEPTH_TEST); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW); glPopMatrix();
        SDL_GL_SwapWindow(win); SDL_Delay(16);
    }
    return 0;
}
#else
int main() { EmpireState s; empire_load("empire.json", &s); empire_generate_site(&s); return 0; }
#endif

```

### FILE: ./services/game-server/src/heartbeat.h
```cpp
#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

// Simple TCP sender
void send_heartbeat_to_master(const char* master_ip, int master_port, int my_port, const char* name, int players, int max) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(master_port);
    inet_pton(AF_INET, master_ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        // Silently fail if master is down
        #ifdef _WIN32
        closesocket(sock);
        #else
        close(sock);
        #endif
        return;
    }

    char msg[256];
    // Protocol: HEARTBEAT|Port|Name|Players|Max
    sprintf(msg, "HEARTBEAT|%d|%s|%d|%d\n", my_port, name, players, max);
    send(sock, msg, strlen(msg), 0);

    #ifdef _WIN32
    closesocket(sock);
    #else
    close(sock);
    #endif
}

#endif

```

### FILE: ./services/game-server/src/server.c
```cpp
#define _WIN32_WINNT 0x0600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h> 

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
#endif

void sleep_ms(int ms) {
    #ifdef _WIN32
    Sleep(ms);
    #else
    usleep(ms * 1000);
    #endif
}

#include "heartbeat.h"
#include "../../../packages/protocol/protocol.h"
#include "../../../packages/map/map.h"
#include "../../../packages/map/map.c"

ServerState state;
GameMap level;
struct sockaddr_in clients[MAX_CLIENTS];
int client_connected[MAX_CLIENTS] = {0};
int demo_mode = 0; // NEW: Bot Flag

#define HISTORY_SIZE 64
typedef struct { int valid; Vec3 pos[MAX_CLIENTS]; } WorldSnapshot;
WorldSnapshot history[HISTORY_SIZE];
Vec3 backup_pos[MAX_CLIENTS];

float rand_f() { return ((float)(rand()%1000)/500.0f) - 1.0f; }
Vec3 get_aim_vector(float yaw, float pitch, float spread) {
    float ry = yaw * (M_PI/180.0f); float rp = pitch * (M_PI/180.0f);
    Vec3 fwd = { sin(ry)*cos(rp), sin(rp), cos(ry)*cos(rp) };
    if (spread > 0) { fwd.x+=rand_f()*spread; fwd.y+=rand_f()*spread; fwd.z+=rand_f()*spread; }
    return fwd;
}
int ray_hit_sphere(Vec3 origin, Vec3 dir, Vec3 target, float radius) {
    Vec3 oc = { target.x-origin.x, target.y-origin.y, target.z-origin.z };
    float t = (oc.x*dir.x + oc.y*dir.y + oc.z*dir.z);
    if (t < 0) return 0;
    Vec3 p = { origin.x + dir.x*t, origin.y + dir.y*t, origin.z + dir.z*t };
    return (powf(p.x-target.x, 2) + powf(p.y-target.y, 2) + powf(p.z-target.z, 2)) < (radius*radius);
}
void record_snapshot() {
    int idx = state.server_tick % HISTORY_SIZE; history[idx].valid = 1;
    for(int i=0; i<MAX_CLIENTS; i++) history[idx].pos[i] = state.players[i].pos;
}
void rewind_world(uint32_t target_tick) {
    int age = state.server_tick - target_tick;
    if (age < 0 || age >= HISTORY_SIZE) return;
    int idx = target_tick % HISTORY_SIZE;
    if (!history[idx].valid) return;
    for(int i=0; i<MAX_CLIENTS; i++) { backup_pos[i] = state.players[i].pos; state.players[i].pos = history[idx].pos[i]; }
}
void restore_world() {
    for(int i=0; i<MAX_CLIENTS; i++) state.players[i].pos = backup_pos[i];
}

void spawn_grenade(int pid) {
    PlayerState *p = &state.players[pid];
    if (p->grenades <= 0) return;
    p->grenades--;
    int gid = -1; for(int i=0; i<MAX_GRENADES; i++) if(!state.grenades[i].active) { gid=i; break; }
    if (gid == -1) return;
    GrenadeState *g = &state.grenades[gid];
    g->active = 1; g->owner_id = pid; g->timer = 120;
    g->pos = p->pos; g->pos.y += 1.5f;
    Vec3 aim = get_aim_vector(p->yaw, p->pitch, 0);
    g->vel.x = aim.x * 0.8f + p->vel.x; g->vel.y = aim.y * 0.8f + 0.2f; g->vel.z = aim.z * 0.8f + p->vel.z;
}

void update_grenades() {
    for(int i=0; i<MAX_GRENADES; i++) {
        GrenadeState *g = &state.grenades[i];
        if (!g->active) continue;
        g->pos.x += g->vel.x; g->pos.y += g->vel.y; g->pos.z += g->vel.z;
        g->vel.y -= 0.02f;
        if (g->pos.y < 0) { g->pos.y = 0; g->vel.y *= -0.6f; g->vel.x *= 0.8f; g->vel.z *= 0.8f; }
        if (map_check_point(&level, g->pos.x, g->pos.y, g->pos.z)) { g->vel.x *= -0.5f; g->vel.z *= -0.5f; }
        g->timer--;
        if (g->timer <= 0) {
            g->active = 0;
            for(int p=0; p<MAX_CLIENTS; p++) {
                if(!state.players[p].active) continue;
                float d = sqrtf(powf(state.players[p].pos.x - g->pos.x, 2) + powf(state.players[p].pos.z - g->pos.z, 2));
                if (d < 5.0f) {
                    state.players[p].health -= (int)(100 * (1.0f - d/5.0f));
                    state.players[p].vel.y += 0.5f;
                    if(state.players[p].health <= 0) {
                        state.players[p].deaths++; state.players[g->owner_id].kills++;
                        state.players[p].health = 100; state.players[p].shield = 100;
                        state.players[p].pos.x = rand_f()*20; state.players[p].pos.z = rand_f()*20;
                    }
                }
            }
        }
    }
}

void process_shoot(int pid, uint32_t render_tick) {
    PlayerState *p = &state.players[pid];
    int w = p->current_weapon;
    if (p->attack_cooldown > 0) return;
    if (w != WPN_KNIFE && p->ammo[w] <= 0) { p->attack_cooldown = 60; p->ammo[w] = WPN_STATS[w].ammo_max; return; }
    
    rewind_world(render_tick);
    if(w != WPN_KNIFE) p->ammo[w]--;
    p->attack_cooldown = WPN_STATS[w].rof;
    p->is_shooting = 5;
    
    int pellets = WPN_STATS[w].cnt; float range = (w == WPN_KNIFE) ? 2.5f : 200.0f;
    Vec3 origin = { p->pos.x, p->pos.y + 1.5f, p->pos.z };

    for (int k=0; k<pellets; k++) {
        Vec3 dir = get_aim_vector(p->yaw, p->pitch, WPN_STATS[w].spr);
        if (map_ray_cast(&level, origin, dir, range)) continue;
        for(int i=0; i<MAX_CLIENTS; i++) {
            if (i == pid || !state.players[i].active) continue;
            PlayerState *vic = &state.players[i];
            float dist = sqrtf(powf(vic->pos.x - p->pos.x, 2) + powf(vic->pos.z - p->pos.z, 2));
            if (dist > range) continue;
            if (ray_hit_sphere(origin, dir, vic->pos, 1.0f)) {
                p->hit_feedback = (vic->shield > 0) ? 1 : 2;
                int dmg = WPN_STATS[w].dmg;
                if (vic->shield > 0) { vic->shield -= dmg; if(vic->shield < 0) { vic->health += vic->shield; vic->shield = 0; } } 
                else { vic->health -= dmg; }
                if (vic->health <= 0) { 
                    vic->health = 100; vic->shield = 100; vic->pos.x = rand_f()*20; vic->pos.z = rand_f()*20;
                    vic->deaths++; p->kills++;
                    for(int j=0; j<MAX_WEAPONS; j++) vic->ammo[j] = WPN_STATS[j].ammo_max;
                }
            }
        }
    }
    restore_world();
}

int main(int argc, char* argv[]) {
    #ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif

    // ARG PARSING
    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "--demo") == 0) {
            demo_mode = 1;
            printf("[SERVER] Demo Mode Active: Bots Enabled\n");
        }
    }

    map_init(&level);
    for(int i=0; i<MAX_VEHICLES; i++) { state.vehicles[i].id = i; state.vehicles[i].pos.x = i * 10.0f; state.vehicles[i].pos.z = 10.0f; state.vehicles[i].driver_id = -1; }
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET; addr.sin_port = htons(PORT); addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    
    #ifdef _WIN32
        DWORD timeout = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    #else
        struct timeval tv = {0, 1000};
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    #endif
    
    state.match_timer = MATCH_DURATION;
    time_t last_heartbeat = 0;
    
    printf("SHANK SERVER ONLINE (FIXED) :%d\n", PORT);
    struct sockaddr_in cli; socklen_t len = sizeof(cli); char buf[1024];

    while(1) {
        time_t now = time(NULL);
        if (now - last_heartbeat > 5) {
            int active_count = 0;
            for(int i=0; i<MAX_CLIENTS; i++) if(state.players[i].active) active_count++;
            // Only announce if not demo mode (Keep demo local)
            if (!demo_mode) {
                send_heartbeat_to_master("127.0.0.1", 8080, PORT, "ShankArena-1", active_count, MAX_CLIENTS);
            }
            last_heartbeat = now;
        }

        state.server_tick++;
        state.match_timer--;
        if (state.match_timer <= 0) {
            state.match_timer = MATCH_DURATION;
            for(int i=0; i<MAX_CLIENTS; i++) { state.players[i].kills = 0; state.players[i].deaths = 0; state.players[i].health = 100; }
        }

        // --- BOT LOGIC ---
        if (demo_mode) {
            int bot_id = 7; // Reserved ID
            if (!state.players[bot_id].active) {
                state.players[bot_id].active = 1;
                state.players[bot_id].health = 100;
                state.players[bot_id].pos.z = 10.0f;
            }
            // Strafe left/right
            state.players[bot_id].vel.x = sinf(state.server_tick * 0.05f) * 0.2f;
            state.players[bot_id].yaw += 1.0f; // Spin
            
            if (state.players[bot_id].health <= 0) {
                state.players[bot_id].health = 100;
                state.players[bot_id].pos.x = 0;
                state.players[bot_id].pos.z = 10.0f;
            }
        }

        int n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&cli, &len);
        if (n > 0) {
            ClientPacket *pkt = (ClientPacket*)buf;
            int pid = 0; 
            if (!state.players[pid].active) {
                state.players[pid].active = 1; state.players[pid].health = 100; state.players[pid].shield = 100; state.players[pid].grenades = 3;
                client_connected[pid] = 1; clients[pid] = cli;
            }
            PlayerState *p = &state.players[pid];
            float r = pkt->yaw * (M_PI/180.0f);
            if (p->vehicle_id == -1) { p->vel.x += (pkt->fwd*cos(r) - pkt->strafe*sin(r)) * 0.05f; p->vel.z += (pkt->fwd*sin(r) + pkt->strafe*cos(r)) * 0.05f; }
            p->yaw = pkt->yaw; p->pitch = pkt->pitch;
            if (pkt->weapon_req >= 0) p->current_weapon = pkt->weapon_req;
            if (pkt->shoot) process_shoot(pid, pkt->render_tick);
            if (pkt->throw_grenade) spawn_grenade(pid);
            if (pkt->interact) {
                if (p->vehicle_id == -1) {
                    for(int v=0; v<MAX_VEHICLES; v++) {
                        float d = sqrtf(powf(state.vehicles[v].pos.x - p->pos.x, 2) + powf(state.vehicles[v].pos.z - p->pos.z, 2));
                        if (d < 3.0f && state.vehicles[v].driver_id == -1) {
                            state.vehicles[v].driver_id = pid; p->vehicle_id = v; p->pos = state.vehicles[v].pos;
                            break;
                        }
                    }
                } else {
                    state.vehicles[p->vehicle_id].driver_id = -1; p->vehicle_id = -1; p->pos.y += 2.0f;
                }
            }
        }

        update_grenades();
        for(int i=0; i<MAX_VEHICLES; i++) if(state.vehicles[i].driver_id != -1) state.vehicles[i].pos = state.players[state.vehicles[i].driver_id].pos;

        for(int i=0; i<MAX_CLIENTS; i++) {
            if(!state.players[i].active) continue;
            PlayerState *p = &state.players[i];
            if(p->vehicle_id == -1) { p->pos.x += p->vel.x; p->pos.z += p->vel.z; map_resolve_collision(&level, &p->pos, &p->vel); p->vel.x *= 0.9f; p->vel.z *= 0.9f; }
            else { p->pos = state.vehicles[p->vehicle_id].pos; }
            if (p->attack_cooldown > 0) p->attack_cooldown--;
            if (p->is_shooting > 0) p->is_shooting--;
            p->hit_feedback = 0;
        }
        record_snapshot();
        for(int i=0; i<MAX_CLIENTS; i++) if(client_connected[i]) sendto(sockfd, (const char*)&state, sizeof(state), 0, (struct sockaddr*)&clients[i], sizeof(clients[i]));
        sleep_ms(16);
    }
}

```

### FILE: ./packages/protocol/protocol.h
```cpp
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define PORT 5314 
#define MAX_CLIENTS 10
#define MAX_WEAPONS 5

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
    int active;
    Vec3 pos; Vec3 vel;
    float yaw, pitch, roll;
    int driver_id;
} VehicleState;

typedef struct {
    int active;
    char name[32];
    Vec3 pos; Vec3 vel;
    float yaw; float pitch;
    int health; int kills;
    int current_weapon; int ammo[MAX_WEAPONS];
    int attack_cooldown; int is_shooting;
    int hit_feedback; int color; int reload_timer;
    int in_vehicle;
    float ground_friction; // <-- NEW: Current traction
} PlayerState;

typedef struct {
    int server_tick;
    int player_count;
    char status_msg[64];
    PlayerState players[MAX_CLIENTS];
    VehicleState vehicle;
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
static WeaponStats WPN_STATS[MAX_WEAPONS] = {
    {0, 45, 20, 1, 0.0f, 0, 3.5f}, {1, 24, 12, 1, 0.0f, 12, 200.0f}, 
    {2, 12, 6, 1, 0.04f, 30, 200.0f}, {3, 10, 50, 12, 0.22f, 8, 70.0f}, {4, 95, 70, 1, 0.0f, 5, 500.0f}
};

#endif

```

### FILE: ./packages/map/map.c
```cpp
#include <math.h>
#include "map.h" 

void map_init(GameMap *map) {
    map->wall_count = 0;
    
    // 1. THE FLOOR (Red Desert Sand)
    // ID 1 = Base Floor (Medium Grip)
    map->walls[map->wall_count++] = (Wall){1, 0, -1.0f, 0, 300.0f, 2.0f, 300.0f, 0.6f, 0.3f, 0.1f, 0.85f}; 

    srand(2077); // Seed for Canyon

    // 2. THE MESAS (Towering Red Rocks)
    // ID 2 = High Grip Rock (Climbable)
    for (int i=0; i<50; i++) {
        float angle = (i / 50.0f) * 6.28f;
        float dist = 50.0f + (rand()%20); 
        float x = sinf(angle) * dist;
        float z = cosf(angle) * dist;
        float h = 15.0f + (rand()%15); 
        float w = 10.0f + (rand()%10);
        
        map->walls[map->wall_count++] = (Wall){
            2, 
            x, h/2.0f - 2.0f, z, 
            w, h, w, 
            0.5f, 0.2f, 0.1f, 0.80f // GRIPPY (0.80 friction)
        };
    }

    // 3. THE BOULDERS (Slippery Scree)
    // ID 3 = Slippery Rock (Ice Physics)
    for(int i=0; i<40; i++) {
        float angle = (i / 40.0f) * 6.28f;
        float dist = 40.0f - (rand()%10);
        float x = sinf(angle) * dist;
        float z = cosf(angle) * dist;
        float h = 2.0f + (rand() % 4);
        
        map->walls[map->wall_count++] = (Wall){
            3, 
            x, h/2.0f, z, 
            3.0f, h, 3.0f, 
            0.4f, 0.4f, 0.4f, 0.98f // SLIPPERY (0.98 friction)
        };
    }
    
    // 4. Central Outpost
    map->walls[map->wall_count++] = (Wall){2, 0, 1.0f, 0, 10.0f, 2.0f, 10.0f, 0.4f, 0.3f, 0.2f, 0.85f};
    map->walls[map->wall_count++] = (Wall){2, 0, 3.0f, 0, 6.0f, 2.0f, 6.0f, 0.4f, 0.3f, 0.2f, 0.85f};
}

int map_ray_cast(GameMap *map, Vec3 origin, Vec3 dir, float max_dist) {
    for(int i=0; i<map->wall_count; i++) {
        Wall *w = &map->walls[i];
        float min_x = w->x - w->sx/2; float max_x = w->x + w->sx/2;
        float min_y = w->y - w->sy/2; float max_y = w->y + w->sy/2;
        float min_z = w->z - w->sz/2; float max_z = w->z + w->sz/2;

        float t1 = (min_x - origin.x)/dir.x; float t2 = (max_x - origin.x)/dir.x;
        float t3 = (min_y - origin.y)/dir.y; float t4 = (max_y - origin.y)/dir.y;
        float t5 = (min_z - origin.z)/dir.z; float t6 = (max_z - origin.z)/dir.z;

        float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
        float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

        if (tmax < 0 || tmin > tmax) continue;
        if (tmin < max_dist) return 1;
    }
    return 0;
}

float map_resolve_collision(GameMap *map, Vec3 *pos, Vec3 *vel) {
    float player_w = 0.6f;
    float player_h = 1.8f; 
    float detected_friction = -1.0f; 

    for(int i=0; i<map->wall_count; i++) {
        Wall *w = &map->walls[i];
        float half_sx = w->sx/2 + player_w/2;
        float half_sy = w->sy/2 + player_h/2;
        float half_sz = w->sz/2 + player_w/2;
        
        float dx = pos->x - w->x;
        float dy = (pos->y + player_h/2) - w->y; 
        float dz = pos->z - w->z;
        
        if (fabs(dx) < half_sx && fabs(dz) < half_sz && fabs(dy) < half_sy) {
            float wall_top = w->y + w->sy/2;
            
            if (vel->y <= 0 && pos->y >= (wall_top - 0.2f)) {
                pos->y = wall_top;
                vel->y = 0;
                detected_friction = w->friction; 
            } else {
                float pen_x = half_sx - fabs(dx);
                float pen_z = half_sz - fabs(dz);
                if (pen_x < pen_z) pos->x = (dx > 0) ? w->x + half_sx : w->x - half_sx;
                else pos->z = (dz > 0) ? w->z + half_sz : w->z - half_sz;
            }
        }
    }
    
    if (pos->y < -20.0f) { pos->y = 10.0f; pos->x = 0; pos->z = 0; vel->y = 0; }
    return detected_friction;
}

```

### FILE: ./packages/map/map.h
```cpp
#ifndef MAP_H
#define MAP_H

#include "protocol.h" // Needs Vec3

typedef struct {
    int id;
    float x, y, z;
    float sx, sy, sz;
    float r, g, b;
    float friction;
} Wall;

typedef struct {
    Wall walls[1000]; // Increased limit for canyon
    int wall_count;
} GameMap;

void map_init(GameMap *map);
int map_ray_cast(GameMap *map, Vec3 origin, Vec3 dir, float max_dist);

// FIXED SIGNATURE: Returns float (friction), not void
float map_resolve_collision(GameMap *map, Vec3 *pos, Vec3 *vel);

#endif

```

### FILE: ./packages/common/config_loader.h
```cpp
#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char master_ip[32];
    int master_port;
    int game_port;
    char player_name[32];
    float sensitivity;
    int width, height;
} GameConfig;

void load_config(const char* filename, GameConfig* cfg) {
    // Defaults
    strcpy(cfg->master_ip, "127.0.0.1");
    cfg->master_port = 8080;
    cfg->game_port = 6969;
    strcpy(cfg->player_name, "Guest");
    cfg->sensitivity = 0.1f;
    cfg->width = 1280;
    cfg->height = 720;

    FILE* f = fopen(filename, "r");
    if (!f) return;

    char line[128];
    while (fgets(line, sizeof(line), f)) {
        char key[64], val[64];
        if (sscanf(line, "%[^=]=%s", key, val) == 2) {
            if (strcmp(key, "MasterIP") == 0) strcpy(cfg->master_ip, val);
            if (strcmp(key, "MasterPort") == 0) cfg->master_port = atoi(val);
            if (strcmp(key, "GamePort") == 0) cfg->game_port = atoi(val);
            if (strcmp(key, "PlayerName") == 0) strcpy(cfg->player_name, val);
            if (strcmp(key, "Sensitivity") == 0) cfg->sensitivity = atof(val);
            if (strcmp(key, "Width") == 0) cfg->width = atoi(val);
            if (strcmp(key, "Height") == 0) cfg->height = atoi(val);
        }
    }
    fclose(f);
}

#endif

```

### FILE: ./packages/common/font.h
```cpp
#ifndef SHANK_FONT_H
#define SHANK_FONT_H
#include <GL/gl.h>
#include <string.h>

// Shared ShankEngine Text Renderer
void shank_draw_text(const char* text, float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    // Simple placeholder for text flow validation
    // In final builds, this maps to the bitmap texture logic
    glBegin(GL_QUADS);
    for(size_t i=0; i<strlen(text); i++) {
        glVertex2f(x + (i*size*0.8f), y);
        glVertex2f(x + (i*size*0.8f) + size*0.6f, y);
        glVertex2f(x + (i*size*0.8f) + size*0.6f, y + size);
        glVertex2f(x + (i*size*0.8f), y + size);
    }
    glEnd();
}
#endif

```

### FILE: ./packages/simulation/game_physics.h
```cpp
#ifndef GAME_PHYSICS_H
#define GAME_PHYSICS_H

#undef ADD_WALL 
#include <math.h>
#include <string.h> 
#include <stdlib.h> 
#include "../protocol/protocol.h"
#include "../map/map.h"

// CONSTANTS
#define GRAVITY 0.025f
#define JUMP_POWER 0.45f
#define MAX_SPEED 0.75f  
#define FRICTION 0.82f
#define ACCEL 1.5f       
#define RELOAD_TIME 60
#define GAME_DURATION 7200 
#define KILL_LIMIT 20

// HELPER MATH
float phys_rand_f() { return ((float)(rand()%1000)/500.0f) - 1.0f; }
Vec3 phys_get_aim(float yaw, float pitch, float spread) {
    float r = -yaw * (M_PI/180.0f);
    float rp = pitch * (M_PI/180.0f);
    Vec3 fwd = { sin(r)*cos(rp), sin(rp), -cos(r)*cos(rp) }; 
    if (spread > 0) { fwd.x+=phys_rand_f()*spread; fwd.y+=phys_rand_f()*spread; fwd.z+=phys_rand_f()*spread; }
    return fwd;
}
int phys_ray_hit(Vec3 origin, Vec3 dir, Vec3 target, float radius) {
    Vec3 oc = { target.x-origin.x, target.y-origin.y, target.z-origin.z };
    float t = (oc.x*dir.x + oc.y*dir.y + oc.z*dir.z);
    if (t < 0) return 0;
    Vec3 p = { origin.x + dir.x*t, origin.y + dir.y*t, origin.z + dir.z*t };
    return (powf(p.x-target.x, 2) + powf(p.y-target.y, 2) + powf(p.z-target.z, 2)) < (radius*radius);
}

// MAP INIT
void phys_init_map(GameMap *map) {
    map->wall_count = 0;
    #define ADD_WALL(x, y, z, w, h, d, cr, cg, cb) map->walls[map->wall_count++] = (Wall){x, y, z, w, h, d, cr, cg, cb}
    ADD_WALL(5, 0.25f, 5, 1.0f, 0.25f, 1.0f, 1.0f, 0.5f, 0.0f);
    ADD_WALL(6, 0.75f, 5, 1.0f, 0.75f, 1.0f, 1.0f, 0.6f, 0.0f); 
}

// MOVE LOGIC
void phys_apply_friction(PlayerState *p) {
    float speed = sqrtf(p->vel.x*p->vel.x + p->vel.z*p->vel.z);
    if (speed < 0.001f || p->pos.y > 0.001f) return;
    float drop = ((speed < 0.1f) ? 0.1f : speed) * FRICTION;
    float newspeed = speed - drop;
    if (newspeed < 0) newspeed = 0;
    newspeed /= speed;
    p->vel.x *= newspeed; p->vel.z *= newspeed;
}

void phys_accelerate(PlayerState *p, float wish_x, float wish_z, float wish_speed, float accel) {
    float current_speed = (p->vel.x * wish_x) + (p->vel.z * wish_z);
    float add_speed = wish_speed - current_speed;
    if (add_speed <= 0) return;
    float acc_speed = accel * wish_speed * 0.1f;
    if (acc_speed > add_speed) acc_speed = add_speed;
    p->vel.x += acc_speed * wish_x; p->vel.z += acc_speed * wish_z;
}

void phys_slide_move(GameMap *map, Vec3 *pos, Vec3 *vel, int is_crouching) {
    float w_sz = 0.3f; float h_sz = is_crouching ? 1.0f : 1.8f; 
    for (int i=0; i < map->wall_count; i++) {
        Wall *w = &map->walls[i];
        if (pos->x + w_sz > w->x - w->sx && pos->x - w_sz < w->x + w->sx &&
            pos->z + w_sz > w->z - w->sz && pos->z - w_sz < w->z + w->sz &&
            pos->y < w->y + w->sy && (pos->y + h_sz) > w->y - w->sy) 
        {
            float dx1 = (w->x + w->sx) - (pos->x - w_sz); float dx2 = (pos->x + w_sz) - (w->x - w->sx);
            float dz1 = (w->z + w->sz) - (pos->z - w_sz); float dz2 = (pos->z + w_sz) - (w->z - w->sz);
            float dy1 = (w->y + w->sy) - pos->y; float dy2 = (pos->y + h_sz) - (w->y - w->sy); 
            float ox = (fabs(dx1) < fabs(dx2)) ? dx1 : -dx2; float oz = (fabs(dz1) < fabs(dz2)) ? dz1 : -dz2;
            if (dy1 > 0 && dy1 < 0.5f && vel->y <= 0) { pos->y += dy1; vel->y = 0; } 
            else if (dy2 > 0 && dy2 < 0.5f && vel->y > 0) { pos->y -= dy2; vel->y = 0; }
            else { if (fabs(ox) < fabs(oz)) { pos->x += ox; vel->x = 0; } else { pos->z += oz; vel->z = 0; } }
        }
    }
}

void phys_update_player(GameMap *map, PlayerState *p, float fwd, float strafe, float yaw, float pitch, int jump, int crouch) {
    p->yaw = yaw; p->pitch = pitch;
    phys_apply_friction(p);

    float r = -yaw * (M_PI/180.0f);
    float fwd_x = sinf(r); float fwd_z = -cosf(r); 
    float right_x = cosf(r); float right_z = sinf(r); 
    float wish_x = (fwd_x * fwd) + (right_x * strafe);
    float wish_z = (fwd_z * fwd) + (right_z * strafe);
    float wish_len = sqrtf(wish_x*wish_x + wish_z*wish_z);
    if (wish_len > 0) { wish_x /= wish_len; wish_z /= wish_len; }

    float target_speed = crouch ? (MAX_SPEED * 0.5f) : MAX_SPEED;
    int grounded = (p->pos.y <= 0.001f);
    
    if (grounded) { if (wish_len > 0) phys_accelerate(p, wish_x, wish_z, target_speed, ACCEL); } 
    else { if (wish_len > 0) phys_accelerate(p, wish_x, wish_z, target_speed, ACCEL * 0.1f); }

    p->vel.y -= GRAVITY;
    if (p->pos.y <= 0) { p->pos.y = 0; p->vel.y = 0; grounded = 1; }
    if (p->pos.y < -10.0f) { p->pos.x = 0; p->pos.y = 5.0f; p->pos.z = 0; p->vel.x=0; p->vel.y=0; p->vel.z=0; }

    phys_slide_move(map, &p->pos, &p->vel, crouch);
    if (p->vel.y == 0 && p->pos.y > 0) grounded = 1; 

    if (jump && grounded) { p->vel.y = JUMP_POWER; p->pos.y += 0.05f; }

    p->pos.x += p->vel.x; p->pos.z += p->vel.z; p->pos.y += p->vel.y;
    
    if (p->reload_timer > 0) {
        p->reload_timer--;
        if (p->reload_timer == 0) p->ammo[p->current_weapon] = WPN_STATS[p->current_weapon].ammo_max;
    }
    if (p->attack_cooldown > 0) p->attack_cooldown--;
    if (p->is_shooting > 0) p->is_shooting--;
    p->hit_feedback = 0; 
}
#endif

```

### FILE: ./packages/simulation/local_game.h
```cpp
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

```

### FILE: ./ops/genesis/genesis.json
```cpp
{
  "chain_id": "skatechain-1",
  "genesis_time": "2026-01-24T12:00:00Z",
  "consensus": {
    "pow_difficulty": 1000,
    "block_time": 10
  },
  "allocations": {
    "skate1_foundation": 1000000000,
    "skate1_dev_fund": 50000000,
    "skate1_faucet": 1000000
  }
}
```

### FILE: ./utils/cli/deploy_server.py
```cpp

import os
import subprocess
import sys
import argparse
import time

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../../"))
BIN_DIR = os.path.join(PROJECT_ROOT, "bin")

DEFAULT_HOST = "s.farthq.com"
DEFAULT_USER = "root"
DEFAULT_KEY = os.path.join(PROJECT_ROOT, ".ssh", "id_rsa")
REMOTE_DIR = "/root/shankpit"

GREEN = "\033[92m"
CYAN = "\033[96m"
RESET = "\033[0m"

def run_cmd(cmd, shell=False, check=True):
    try:
        return subprocess.run(cmd, shell=shell, check=check, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    except subprocess.CalledProcessError as e:
        print(f"❌ COMMAND FAILED: {e.cmd}")
        print(e.stderr)
        sys.exit(1)

def deploy(host, user, key_path):
    print(f"⚔️  DEPLOYING TO {host}...")

    # 1. BUILD FIRST (Get Version)
    print("🔨 Triggering Build...")
    res = run_cmd(f"python {os.path.join(SCRIPT_DIR, 'build_server.py')}", shell=True)
    
    # Parse output for VERSION_TAG=...
    version_tag = "ShankServer_Latest" # Fallback
    for line in res.stdout.splitlines():
        if line.startswith("VERSION_TAG="):
            version_tag = line.split("=")[1].strip()
            print(f"   -> Detected Build Version: {CYAN}{version_tag}{RESET}")

    local_binary = os.path.join(BIN_DIR, version_tag)

    # 2. PREPARE KEY
    temp_key = "/tmp/shank_deploy_key"
    run_cmd(f"cp '{key_path}' {temp_key} && chmod 600 {temp_key}", shell=True)
    ssh_opts = f"-i {temp_key} -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null"

    # 3. UPLOAD
    print(f"🚀 Uploading {version_tag}...")
    run_cmd(f"ssh {ssh_opts} {user}@{host} 'mkdir -p {REMOTE_DIR}'", shell=True)
    run_cmd(f"scp {ssh_opts} {local_binary} {user}@{host}:{REMOTE_DIR}/{version_tag}", shell=True)

    # 4. SWAP & RESTART
    print(f"🔄 Switching Version & Restarting...")
    remote_cmds = [
        f"chmod +x {REMOTE_DIR}/{version_tag}",
        "pkill -f ShankServer || true", 
        f"ln -sf {REMOTE_DIR}/{version_tag} {REMOTE_DIR}/ShankServer_Current",
        f"nohup {REMOTE_DIR}/ShankServer_Current > {REMOTE_DIR}/server.log 2>&1 &"
    ]
    run_cmd(f"ssh {ssh_opts} {user}@{host} '{' && '.join(remote_cmds)}'", shell=True)

    print(f"{GREEN}✅ DEPLOY SUCCESSFUL.{RESET}")
    print(f"   Active Version: {version_tag}")
    os.remove(temp_key)
    return True

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--user", default=DEFAULT_USER)
    parser.add_argument("--key", default=DEFAULT_KEY)
    args = parser.parse_args()
    deploy(args.host, args.user, args.key)

```

### FILE: ./utils/cli/build_server.py
```cpp

import os
import subprocess
import shutil
import sys
import datetime

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../../"))

SERVER_SRC = os.path.join(PROJECT_ROOT, "apps", "server", "src", "main.c")
BIN_DIR = os.path.join(PROJECT_ROOT, "bin")

# Generate Version Name: ShankServer_vYYYYMMDD_HHMM
VERSION = datetime.datetime.now().strftime("v%Y%m%d_%H%M")
BINARY_NAME = f"ShankServer_{VERSION}"
OUTPUT_BINARY = os.path.join(BIN_DIR, BINARY_NAME)

INC_PROTO = os.path.join(PROJECT_ROOT, "packages", "protocol")
INC_SIM = os.path.join(PROJECT_ROOT, "packages", "simulation")
INC_MAP = os.path.join(PROJECT_ROOT, "packages", "map")

def build_server():
    print(f"🔨 BUILDING {BINARY_NAME}...")
    
    if not os.path.exists(SERVER_SRC):
        print(f"❌ Source missing: {SERVER_SRC}")
        return None

    if not os.path.exists(BIN_DIR):
        os.makedirs(BIN_DIR)

    cmd = [
        "gcc", SERVER_SRC,
        "-o", OUTPUT_BINARY,
        "-O2", "-static", "-static-libgcc",
        f"-I{INC_PROTO}", f"-I{INC_SIM}", f"-I{INC_MAP}",
        "-lm"
    ]
    
    try:
        subprocess.run(cmd, check=True)
        print(f"✅ COMPILE SUCCESS: {OUTPUT_BINARY}")
        
        # Also create a 'latest' symlink/copy for ease of use
        latest = os.path.join(BIN_DIR, "ShankServer_Latest")
        shutil.copy(OUTPUT_BINARY, latest)
        
        return BINARY_NAME
    except subprocess.CalledProcessError as e:
        print(f"❌ COMPILE FAILED: {e}")
        return None

if __name__ == "__main__":
    res = build_server()
    if res:
        # Print the version name to stdout so deploy script can read it
        print(f"VERSION_TAG={res}")
        sys.exit(0)
    else:
        sys.exit(1)

```

### FILE: ./utils/cli/reset_server.py
```cpp

import os
import subprocess
import sys
import argparse
import time

# --- DEFAULTS ---
DEFAULT_HOST = "s.farthq.com"
DEFAULT_USER = "root"
DEFAULT_KEY = os.path.join(os.path.dirname(os.path.abspath(__file__)), "../../.ssh/id_rsa")
REMOTE_DIR = "/root/shankpit"

GREEN = "\033[92m"
RED = "\033[91m"
CYAN = "\033[96m"
RESET = "\033[0m"

def run_ssh(host, user, key_path, cmd):
    # Prepare Key
    temp_key = "/tmp/shank_rescue_key"
    if not os.path.exists(temp_key):
        subprocess.run(f"cp '{key_path}' {temp_key} && chmod 600 {temp_key}", shell=True)

    ssh_opts = f"-i {temp_key} -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o ConnectTimeout=5"
    full_cmd = f"ssh {ssh_opts} {user}@{host} '{cmd}'"
    
    try:
        res = subprocess.run(full_cmd, shell=True, capture_output=True, text=True)
        return res.stdout.strip(), res.stderr.strip(), res.returncode
    except Exception as e:
        return "", str(e), 1

def rescue(host, user, key_path):
    print(f"{CYAN}1. CHECKING PROCESSES...{RESET}")
    out, err, _ = run_ssh(host, user, key_path, "ps aux | grep ShankServer | grep -v grep")
    if out:
        print(f"   Found running process:\n{out}")
    else:
        print("   No running server found (Server is DOWN).")

    print(f"\n{CYAN}2. FORCE KILLING...{RESET}")
    run_ssh(host, user, key_path, "pkill -9 -f ShankServer")
    time.sleep(1)
    
    # Verify Kill
    out, _, _ = run_ssh(host, user, key_path, "ps aux | grep ShankServer | grep -v grep")
    if out:
        print(f"{RED}   ❌ FAILED TO KILL PROCESS. IT IS A ZOMBIE.{RESET}")
        return False
    else:
        print(f"{GREEN}   ✅ Process Eliminated.{RESET}")

    print(f"\n{CYAN}3. STARTING FRESH (Latest Binary)...{RESET}")
    # Find the latest binary deployed
    start_cmd = f"nohup {REMOTE_DIR}/ShankServer > {REMOTE_DIR}/server.log 2>&1 &"
    out, err, code = run_ssh(host, user, key_path, start_cmd)
    
    time.sleep(2) # Give it a moment to bind

    print(f"\n{CYAN}4. VERIFYING PORT 5000...{RESET}")
    # Check if port 5000 is listening (using netstat or ss)
    out, _, _ = run_ssh(host, user, key_path, "netstat -ulpn | grep :5000")
    if "5000" in out:
        print(f"{GREEN}   ✅ SERVER IS LISTENING ON PORT 5000{RESET}")
        print(f"   {out}")
        
        print(f"\n{CYAN}5. RECENT LOGS:{RESET}")
        out, _, _ = run_ssh(host, user, key_path, f"tail -n 5 {REMOTE_DIR}/server.log")
        print(out)
        return True
    else:
        print(f"{RED}   ❌ SERVER STARTED BUT NOT LISTENING. CHECK LOGS:{RESET}")
        out, _, _ = run_ssh(host, user, key_path, f"cat {REMOTE_DIR}/server.log")
        print(out)
        return False

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--user", default=DEFAULT_USER)
    # Fix key path resolution
    script_root = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(script_root, "../../"))
    key_default = os.path.join(project_root, ".ssh", "id_rsa")
    
    parser.add_argument("--key", default=key_default)
    args = parser.parse_args()

    rescue(args.host, args.user, args.key)

```

### FILE: ./utils/cli/nuclear_deploy.py
```cpp

import os
import subprocess
import sys
import time

# --- PATHS ---
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../../"))
SERVER_SRC = os.path.join(PROJECT_ROOT, "apps", "server", "src", "main.c")
BIN_DIR = os.path.join(PROJECT_ROOT, "bin")
LOCAL_BINARY = os.path.join(BIN_DIR, "ShankServer_Linux")

# --- REMOTE CONFIG ---
HOST = "s.farthq.com"
USER = "root"
KEY_PATH = os.path.join(PROJECT_ROOT, ".ssh", "id_rsa")
REMOTE_DIR = "/root/shankpit"

GREEN = "\033[92m"
CYAN = "\033[96m"
RED = "\033[91m"
RESET = "\033[0m"

def run_local(cmd):
    try:
        subprocess.run(cmd, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"{RED}❌ LOCAL FAIL: {e.cmd}{RESET}")
        sys.exit(1)

def run_ssh(cmd, verbose=True):
    # Setup Key
    temp_key = "/tmp/shank_nuclear_key"
    if not os.path.exists(temp_key):
        subprocess.run(f"cp '{KEY_PATH}' {temp_key} && chmod 600 {temp_key}", shell=True)
    
    ssh_opts = f"-i {temp_key} -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o ConnectTimeout=10"
    full_cmd = f"ssh {ssh_opts} {USER}@{HOST} '{cmd}'"
    
    try:
        res = subprocess.run(full_cmd, shell=True, capture_output=True, text=True)
        if verbose and res.stdout: print(f"   [REMOTE]: {res.stdout.strip()}")
        if res.stderr: print(f"   [STDERR]: {res.stderr.strip()}")
        return res.returncode == 0
    except Exception as e:
        print(f"{RED}❌ SSH FAIL: {e}{RESET}")
        return False

def nuke_and_pave():
    print(f"{CYAN}☢️  INITIATING NUCLEAR DEPLOYMENT SEQUENCE...{RESET}")
    
    # 1. COMPILE LOCALLY
    print(f"\n{CYAN}1. COMPILING FRESH BINARY...{RESET}")
    # Includes
    inc_proto = os.path.join(PROJECT_ROOT, "packages", "protocol")
    inc_sim = os.path.join(PROJECT_ROOT, "packages", "simulation")
    inc_map = os.path.join(PROJECT_ROOT, "packages", "map")
    
    compile_cmd = f"gcc {SERVER_SRC} -o {LOCAL_BINARY} -O2 -static -static-libgcc -I{inc_proto} -I{inc_sim} -I{inc_map} -lm"
    run_local(compile_cmd)
    print(f"{GREEN}   ✅ Compilation Successful.{RESET}")

    # 2. PREPARE KEY
    if not os.path.exists(KEY_PATH):
        print(f"{RED}❌ MISSING KEY: {KEY_PATH}{RESET}")
        return

    # 3. WIPE REMOTE
    print(f"\n{CYAN}2. WIPING REMOTE DIRECTORY...{RESET}")
    run_ssh(f"pkill -9 -f ShankServer") # Kill old
    run_ssh(f"rm -rf {REMOTE_DIR}")     # Delete folder
    run_ssh(f"mkdir -p {REMOTE_DIR}")   # Recreate
    print(f"{GREEN}   ✅ Remote Cleaned.{RESET}")

    # 4. UPLOAD (Force Simple Name)
    print(f"\n{CYAN}3. UPLOADING ARTIFACT...{RESET}")
    temp_key = "/tmp/shank_nuclear_key"
    scp_cmd = f"scp -i {temp_key} -o StrictHostKeyChecking=no {LOCAL_BINARY} {USER}@{HOST}:{REMOTE_DIR}/ShankServer"
    run_local(scp_cmd)
    print(f"{GREEN}   ✅ Upload Complete.{RESET}")

    # 5. EXECUTE
    print(f"\n{CYAN}4. LAUNCHING SERVER...{RESET}")
    # Chmod + Run
    start_cmd = f"chmod +x {REMOTE_DIR}/ShankServer && nohup {REMOTE_DIR}/ShankServer > {REMOTE_DIR}/server.log 2>&1 &"
    run_ssh(start_cmd)
    
    time.sleep(2)

    # 6. VERIFY
    print(f"\n{CYAN}5. VERIFICATION:{RESET}")
    print("   Checking Process...")
    run_ssh("ps aux | grep ShankServer | grep -v grep")
    
    print("   Checking Port 5000...")
    run_ssh("netstat -ulpn | grep :5000")
    
    print("   Checking Logs...")
    run_ssh(f"tail -n 5 {REMOTE_DIR}/server.log")

if __name__ == "__main__":
    nuke_and_pave()

```

### FILE: ./utils/cli/git_sync.py
```cpp

import os
import subprocess
import sys
import datetime

# --- CONFIG ---
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../../"))
KEY_PATH = os.path.join(PROJECT_ROOT, ".ssh", "id_rsa")
REPO_URL = "git@gitlab.com:danowski/skateboard.git"

GREEN = "\033[92m"
RED = "\033[91m"
CYAN = "\033[96m"
YELLOW = "\033[93m"
RESET = "\033[0m"

def run_cmd(cmd, env=None, check=True):
    try:
        res = subprocess.run(cmd, shell=True, env=env, check=check, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        return res.stdout.strip(), res.stderr.strip(), res.returncode
    except subprocess.CalledProcessError as e:
        return e.stdout.strip() if e.stdout else "", e.stderr.strip() if e.stderr else str(e), e.returncode

def sync():
    print(f"{CYAN}📂 GIT SYNC v2 (ANTI-HANG){RESET}")
    
    # 1. SETUP SSH
    temp_key = "/tmp/gitlab_key"
    if not os.path.exists(KEY_PATH):
        print(f"{RED}❌ SSH Key missing at {KEY_PATH}{RESET}")
        return

    subprocess.run(f"cp '{KEY_PATH}' {temp_key} && chmod 600 {temp_key}", shell=True)
    
    # BatchMode=yes prevents password prompts (which cause hangs)
    git_env = os.environ.copy()
    git_env["GIT_SSH_COMMAND"] = f"ssh -i {temp_key} -o StrictHostKeyChecking=no -o BatchMode=yes"

    # 2. INIT
    if not os.path.exists(os.path.join(PROJECT_ROOT, ".git")):
        print(f"   Initializing Repo...")
        run_cmd(f"git -C {PROJECT_ROOT} init", env=git_env)
        run_cmd(f"git -C {PROJECT_ROOT} remote add origin {REPO_URL}", env=git_env)
    
    run_cmd(f"git -C {PROJECT_ROOT} remote set-url origin {REPO_URL}", env=git_env)
    run_cmd(f"git -C {PROJECT_ROOT} config user.email 'skatebot@farthq.com'", env=git_env)
    run_cmd(f"git -C {PROJECT_ROOT} config user.name 'SkateChain Bot'", env=git_env)

    # 3. COMMIT
    run_cmd(f"git -C {PROJECT_ROOT} add .", env=git_env)
    status, _, _ = run_cmd(f"git -C {PROJECT_ROOT} status --porcelain", env=git_env)
    if status:
        ts = datetime.datetime.now().strftime("%Y-%m-%d %H:%M")
        print(f"   Committing changes...")
        run_cmd(f"git -C {PROJECT_ROOT} commit -m 'Auto-save {ts}'", env=git_env)

    # 4. PUSH
    print(f"🚀 Pushing to GitLab...")
    out, err, code = run_cmd(f"git -C {PROJECT_ROOT} push -u origin master", env=git_env, check=False)

    if code == 0:
        print(f"{GREEN}✅ SYNC COMPLETE.{RESET}")
    else:
        print(f"{RED}❌ PUSH FAILED (Auth Error?){RESET}")
        if "Permission denied" in err:
            print(f"👉 YOUR PUBLIC KEY (Add to GitLab):")
            subprocess.run(f"ssh-keygen -y -f {temp_key}", shell=True)
        else:
            print(f"Error: {err}")

if __name__ == "__main__":
    sync()

```

### FILE: ./utils/cli/deploy_hybrid.py
```cpp

import os
import subprocess
import sys
import time

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../../"))
SERVER_SRC = os.path.join(PROJECT_ROOT, "apps", "server", "src", "main.c")
BIN_DIR = os.path.join(PROJECT_ROOT, "bin")
LOCAL_BINARY = os.path.join(BIN_DIR, "ShankServer_Linux")

HOST = "s.farthq.com"
USER = "root"
KEY_PATH = os.path.join(PROJECT_ROOT, ".ssh", "id_rsa")
REMOTE_DIR = "/root/shankpit"

GREEN = "\033[92m"
CYAN = "\033[96m"
RESET = "\033[0m"

def run_ssh(cmd):
    temp_key = "/tmp/shank_deploy_key"
    if not os.path.exists(temp_key):
        subprocess.run(f"cp '{KEY_PATH}' {temp_key} && chmod 600 {temp_key}", shell=True)
    
    ssh_opts = f"-i {temp_key} -o StrictHostKeyChecking=no -o ConnectTimeout=10"
    full_cmd = f"ssh {ssh_opts} {USER}@{HOST} '{cmd}'"
    subprocess.run(full_cmd, shell=True)

def deploy():
    print(f"{CYAN}🚀 DEPLOYING HYBRID SERVER...{RESET}")
    
    # 1. Compile
    print("🔨 Compiling Linux Binary...")
    inc_proto = os.path.join(PROJECT_ROOT, "packages", "protocol")
    inc_sim = os.path.join(PROJECT_ROOT, "packages", "simulation")
    inc_map = os.path.join(PROJECT_ROOT, "packages", "map")
    
    cmd = f"gcc {SERVER_SRC} -o {LOCAL_BINARY} -O2 -static -static-libgcc -I{inc_proto} -I{inc_sim} -I{inc_map} -lm"
    subprocess.run(cmd, shell=True, check=True)
    
    # 2. Upload
    print("☁️  Uploading...")
    temp_key = "/tmp/shank_deploy_key"
    subprocess.run(f"scp -i {temp_key} -o StrictHostKeyChecking=no {LOCAL_BINARY} {USER}@{HOST}:{REMOTE_DIR}/ShankServer", shell=True)
    
    # 3. Restart
    print("🔄 Restarting Service...")
    run_ssh(f"pkill -9 -f ShankServer || true")
    run_ssh(f"chmod +x {REMOTE_DIR}/ShankServer")
    run_ssh(f"nohup {REMOTE_DIR}/ShankServer > {REMOTE_DIR}/server.log 2>&1 &")
    
    print(f"{GREEN}✅ DEPLOY COMPLETE.{RESET}")
    print("   You can now connect with [A] (Online) or [D] (Offline)!")

if __name__ == "__main__":
    deploy()

```

### FILE: ./utils/cli/bot_patrol.py
```cpp
import socket
import struct
import time
import random
import threading

# --- CONFIGURATION ---
SERVER_IP = "s.farthq.com"
SERVER_PORT = 5314
BOT_COUNT = 2 
PACKET_INPUT = 1

# Packet Format (Phase 95 MTU Fix + Phase 98 Header)
# Header: int type + int owner_id (Server ignores owner_id from client, but we match size)
# Body: ClientInput struct (fwd, strafe, yaw, pitch, jump, crouch, shoot, reload, wpn, zoom)
PACKET_FMT = "ii" + "ffffiiiiii" 

class Bot:
    def __init__(self, id):
        self.id = id
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setblocking(False)
        self.addr = (SERVER_IP, SERVER_PORT)
        
        self.yaw = 0.0
        self.pitch = 0.0
        self.move_timer = 0
        self.fwd = 0.0
        self.strafe = 0.0
        self.jump = 0
        self.shoot = 0
        self.weapon = 2
        
    def update(self):
        # Slower, more deliberate movement
        self.yaw += 1.5 
        
        self.move_timer += 1
        if self.move_timer > 60: # Change every 3 seconds
            self.move_timer = 0
            self.fwd = 1.0 # Just walk forward in a circle
            self.jump = 1 if random.random() < 0.2 else 0
            self.shoot = 1 if random.random() < 0.3 else 0
            
            # Swap between Rifle (2) and Shotgun (3)
            self.weapon = 3 if self.weapon == 2 else 2

        # Data construction
        # Send Type (1) + OwnerID (0, ignored) + Input Data
        data = struct.pack(PACKET_FMT, 
                           PACKET_INPUT, 0,
                           self.fwd, self.strafe, self.yaw, self.pitch,
                           self.jump, 0, self.shoot, 0, self.weapon, 0)
        
        try:
            self.sock.sendto(data, self.addr)
        except:
            pass

    def listen(self):
        try:
            while True:
                self.sock.recvfrom(4096)
        except BlockingIOError:
            pass

def run_patrol():
    print(f"👮 LAUNCHING PATROL: {BOT_COUNT} BOTS -> {SERVER_IP}:{SERVER_PORT}")
    bots = [Bot(i) for i in range(BOT_COUNT)]
    
    try:
        frame = 0
        while True:
            for bot in bots:
                bot.listen()
                bot.update()
            
            time.sleep(0.05) # 20 ticks/sec
            frame += 1
            if frame % 40 == 0:
                print(f"   [Patrol Active] Bots are walking circles...")
                
    except KeyboardInterrupt:
        print("\n🛑 PATROL STOPPED.")

if __name__ == "__main__":
    run_patrol()

```

### FILE: ./release_server/run.sh
```cpp
#!/bin/bash
chmod +x ShankServer
./ShankServer

```
