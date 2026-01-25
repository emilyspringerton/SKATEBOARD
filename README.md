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



----


🛹 FARTCO AMERICA: THE SKATECHAIN FOUNDRY
AI-Accelerated Monorepo | Fragment The Void

A decentralized polyglot metaverse connecting high-performance C game clients with a Go-based blockchain backend. This is the "Witch Engine" foundry for high-velocity software architecture, procedural graphics, and cross-chain state management.

🏗️ The Architecture Stack
🔗 Blockchain & Services (Go)
Skate Node: Custom Layer-1 Blockchain featuring Hybrid PoW/PoL Consensus and P2P GossipSub.

DNS-Server: Persistent MMO backend for DragonsNShit utilizing BadgerDB for state consistency and TCP for high-reliability world sync.

Bridge Node: ZK-SNARK Prover for cross-chain state root submission to Ethereum.

Master Server: Global Auth (Google OAuth) and Matchmaking.

🎮 Game Clients (C/SDL2/OpenGL)
ShankPit (FPS): Quake-style arena shooter with "Slippery Neon" physics and procedural recoil.

DragonsNShit (MMO): Persistent world exploration with rotoscoped voxel graphics and dual-outline rendering (Town & Forest zones).

Empire of the Worlds: Automation dashboard for managing bots and agents across digital realms.

Fishing: Chill economy loop featuring RNG fairness and inventory persistence.

🌐 Web & Tooling
Static Site Generator (SSG): A lightweight Go-based engine that transforms empire.json and content.json into the public-facing farthq.com dashboard.

Lightweight Web Server: A high-concurrency Go server for serving the SSG output and handling light API requests.

Witch Editor: In-engine, fixed-buffer text editor for live-coding and empire.json manipulation.

Turtle Library: Math-based 2D/3D vector simulation for low-footprint graphics.
