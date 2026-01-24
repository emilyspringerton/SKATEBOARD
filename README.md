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
