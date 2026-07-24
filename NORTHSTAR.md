# NORTHSTAR — [Working Title TBD]: GTA3 × Skate 2, Non-Voxel Destructible Worlds, TrapX Universe

**Status:** Pure northstar. Nothing built yet — docs before software, same discipline as every
other product in this ecosystem. This document exists to capture founder direction accurately
before any implementation starts.

---

## 1. What this is, and — just as important — what it isn't

A new, separate product living in this repo (`skateboard` — the original "SkateChain: The
Polyglot Metaverse" monorepo, the ancestor this whole ecosystem grew out of). Set in the same
fictional universe as **TRAPX** (`SHANKPIT/docs2/TRAPX_NORTHSTAR.md`) — the same city, the same
world-that-doesn't-care-you're-there tone — but a **sibling product, not a replacement**. TRAPX's
own northstar is explicit and unambiguous: "a 3D voxel urban sandbox... built on the GoblinFoxDragon
(DragonsNShit) voxel engine." That stays exactly as it is. This document does not touch it,
does not compete with it for the same engine, and does not ask GFD to become something it isn't.

This game exists because the founder wants a second, technically distinct answer to "what does an
open city feel like to move through" — one built on **non-voxel destructible geometry**, the way
Rainbow Six Siege does it, rather than the blocky, Minecraft-lineage voxel destruction GFD/
DragonsNShit already owns. Two products, same universe, two different bets on how a destructible
city should actually be represented under the hood.

## 2. The three-pillar blend

**GTA3** — open-world structure. A real city to drive/walk/run through, not a level select
screen. Third-person default (matching TRAPX's own camera default, for tonal consistency across
the universe), missions/activities scattered across real geography, vehicles as a first-class
traversal option alongside foot movement.

**Skate 2** — the movement identity, not a minigame bolted onto GTA3's chassis. "Flick-it"-style
analog trick input (not a fixed combo-button system), real momentum and bail physics (falling off
a trick should read as a physics failure, not a canned animation), and — Skate 2's actual defining
idea — **the city itself is the skatepark**. Curbs, rails, gaps, stairsets, rooftops: geography
that exists for GTA3's traversal purposes doing double duty as terrain a skater reads and exploits.
This is also the load-bearing reason destruction matters here in a way it doesn't for a normal
open-world game: a skater's read of a space changes the instant a wall comes down, a ramp gets
blown out, or a rooftop gap opens where there wasn't one a minute ago.

**Non-voxel destructible worlds, R6 Siege-style** — the actual technical pioneering angle, and
the reason this can't just be "TRAPX with a skateboard." Siege's destruction isn't a voxel world;
it's authored geometry with **destruction states** — walls, floors, and ceilings that can be
breached, reinforced, or collapsed, with real line-of-sight and traversal consequences, built on
mesh-based damage/reveal systems rather than a blocky per-voxel world representation. That's the
model to chase here: hand-built city geometry (not procedurally-voxelized) with a destruction
layer bolted onto it — breachable walls open new skate lines and new sightlines at the same time,
the way a breached wall in Siege opens a new angle of attack. Nothing voxel about any of it.

## 3. "Culture" — the actual identity, not just the mechanics

Founder-confirmed framing: this is a **skate/street culture** game before it's a destruction tech
demo. The mechanics above (traversal, tricks, destruction) are in service of that identity, not
the point of it — spots, crews, the culture of skating a city that wasn't built for you, layered
onto TrapX's existing "the city doesn't care you're here" tone. TRAPX's own player fantasy is
becoming the city's most dangerous variable through an RPG class system; this game's fantasy is
closer to claiming a city's geography as your own terrain, culturally and physically, one spot and
one breached wall at a time. Both fantasies can coexist in the same universe because they're
answering different questions about what "the city" means to the person moving through it.

## 4. What's actually here to build from (repo audit, 2026-07-24)

This repo (`skateboard`) already has real, working lineage — it's not a blank slate:

- **`apps/shank-fps`** — the direct ancestor of SHANKPIT itself (FPS movement, net code, particles,
  input handling). Worth checking for any movement/physics code that predates SHANKPIT's own
  current state and might be a cleaner starting point for this game's on-foot movement, since this
  game needs GTA3-style traversal, not SHANKPIT's current arena-shooter feel.
- **`apps/lobby`, `apps/server`** — an earlier lobby/server pair, same lineage as REDGARDEN's and
  SHANKPIT's own client/server split. Auth/matchmaking patterns here likely predate and inform the
  Master Server concept below.
- **Master Server / Skate Node / Bridge Node** (per README's own roadmap) — a custom L1 blockchain
  and ZK-bridge layer were scoped in the original README. **Explicitly not needed for this game's
  actual gameplay loop** — flagged here so a future pass doesn't assume the blockchain layer is a
  prerequisite. Auth/matchmaking (Master Server) is relevant; the chain itself is a separate,
  much later, and much more speculative bet.
- **`shank_ai_v1.pth`** — an early PyTorch checkpoint sitting in the repo root, likely an ancestor
  of SHANKPIT's current trained bot-brain weights (`SHANKPIT/packages/simulation/neural_net.h`).
  Not evaluated yet for relevance to this game specifically — flagged as a real artifact worth a
  look before assuming any bot AI here starts from zero.
- **No existing destructible-geometry system anywhere in this repo, SHANKPIT, or GFD.** This is
  genuinely new work, same as REDGARDEN's shader renderer was genuinely new work when GLU turned
  out to be a dead end worth avoiding rather than fixing — the right comparison isn't "port
  something that exists," it's "there's real design space here nobody's touched yet."

## 5. Explicitly not scoped in this pass

No engine chosen yet (SDL2 + custom, like every other client in this ecosystem, is the likely
default, but not decided). No destruction-mesh implementation, no city geography, no trick-input
scheme, no vehicle list, no mission structure, no faction/crew system. This document is the
direction, not the plan — the actual build plan is a separate pass once the founder confirms this
is the right shape to build toward.
