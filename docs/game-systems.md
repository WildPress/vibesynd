# How the game works, as we map it

The [concept pages](README.md) are about *how* we rebuild the game. This page is
about *what the game's code actually does*: the systems that make Syndicate work,
described as we come to understand each piece.

It's the interesting half of the project. Every function we match is a small window
into how the game was built, and as those windows join up we can start to explain
whole systems: how an agent moves, how a weapon fires, how a mission is scored.

## A note on how early we are

Right now this page is mostly a scaffold. Most of what we've matched so far is either
small utility code or the compiler's [runtime library](game-vs-library.md), where
the game meaning isn't always clear yet. So the sections below are the systems we
*expect* to find, waiting to be filled in. Better to have the shape ready and grow
it than to start from nothing later.

As a function's purpose becomes clear, it gets a note in the manifest and a mention
in the relevant section here. When a system is understood well enough, it gets its
own page.

## The systems we expect to find

- **Agents and other people** — the four agents you control, plus civilians, police,
  and enemy syndicate agents. How they're stored, how they move, take damage, and
  die.
- **Weapons and mods** — the guns and cybernetic modifications, their stats, and what
  happens when one fires.
- **Missions and objectives** — the mission structure, targets, win and loss
  conditions, and the score.
- **The map and rendering** — the isometric world, how it's drawn, and how buildings
  become transparent when you enter them.
- **Pathfinding** — how a person works out a route across the map.
- **Input and the interface** — mouse, keyboard, the mission screen, the menus.
- **Sound and music** — effects and the dynamic soundtrack.
- **Persistence** — saving and loading, and the research and money you carry between
  missions.
- **The platform layer** — the [DOS and hardware](dos-and-dos4gw.md) plumbing:
  graphics modes, the timer, file access.

## The modding angle

This is the longer-term payoff. Once a system is understood, we can explain not just
how it works but how you'd *change* it. For example, when the weapons system is
mapped, a page like "adding a new weapon" could walk through the data and the code
you'd touch to do it. We can't write those yet, because we don't understand the
systems well enough, but that's the direction, and it's why understanding each
function matters beyond just matching its bytes.

## What we've identified so far

An honest, short list. It'll grow.

- **A hardware timer being programmed** (`0x252d8`) — sets up the PC's programmable
  interval timer, part of the low-level platform layer that keeps game time ticking.
- **An object-slot allocator** (`0x22b38`) — scans a fixed table of 256 slots for a
  free one, fills in a few fields, and hands it back. This is the classic shape of an
  "entity pool", so it's likely how the game creates a person, a projectile, or a
  similar game object. Which one, we'll know when we match its callers.
- **Three object pools, and a free-slot scan** (`0x22768`) — this one revealed a big
  piece of the game's memory layout. There are three *contiguous* fixed-size-record
  pools back to back: pool A at `0x8110`, 256 records of 92 bytes; pool B at `0xdd10`,
  64 records of 42 bytes; pool C at `0xe790`, 400 records of 30 bytes. In every record
  the byte at offset `0x18` is an "in use" flag. This function scans each pool from the
  top down and caches a pointer to the free boundary in a global, so later allocations
  are fast. It's called during map setup, which makes sense: loading a map resets the
  world's objects. (Understood; the byte match is parked on register allocation.) The
  92-byte pool A is the same array the flag-scan at `0x14cc8` walks, so pool A is a
  strong candidate for the **agents and people** it manages.
- **The map column-table initialiser** (`0x20d18`) — walks a 12,288-entry table,
  turning each stored offset into an absolute pointer, then publishes the table base in
  `g_5358`. This is the function that *builds* the table the passability check below
  *reads*. Understood but not yet byte-matched (it compiles to an alignment-padded
  loop). Part of the **map and rendering** system.
- **A map passability check** (`0x33fb8`) — given a world position, it finds the map
  tile there and returns whether it's walkable (part of the **map and rendering**
  system). It's understood but not yet byte-matched, it's one register-allocation
  byte short. See the [journal](journal.md).
- **A per-object status update** (`0x2d998`) — recomputes an object's state code from
  its flags, the function that led us to the 9.5 compiler.
- **A chain-length counter** (`0x377b8`) — given an object, it walks a linked chain
  starting from a 16-bit id in one of its fields, following a link at each node, and
  returns how many nodes are in the chain. The classic shape of "count the items in a
  list", so it's likely counting something attached to an object (inventory, a queue,
  or a group). Which one we'll know from its callers. It's byte-matched, and it's the
  function that cracked the [loop-rotation wall](journal.md).
- **A tile-type lookup** (`0x377e8`) — takes an id from an object's field, indexes a
  table to find a tile record, bounds-checks it, and translates a byte from the record
  through a second table into a type code. Part of the **map and rendering** system,
  the same table family as the passability check. Byte-matched.
- **A record-chain walk** (`0x14998`) — follows a linked list of fixed 15-byte records
  (a table indexed by id, each record holding the next id at a fixed offset) until the
  chain ends. Another "walk a list attached to an object" primitive. Byte-matched.
- **Per-type stat initialisation** (`0x20d98`) — for every in-use record in pool B it
  reads a type byte at `+0x19` and writes a value to the word at `+0x14`. The values
  look like hit-points, and the types come in consecutive pairs (likely two variants of
  the same object): `1,2 → 600`, `5,6 → 100`, `9,10 → 80`, `13,14 → 30`, `17,18 → 40`,
  `28,29 → 10`, `36,37 → 120`, `40,41 → 115`. So pool B holds **typed objects with hit
  points** (vehicles or destructible scenery are the likely candidates), and this pass
  stamps each object's starting health from its type when a map loads. This is exactly
  the kind of table you'd edit to mod object toughness. (Understood; the byte match is
  parked on Watcom's switch-tree balancing.)
- **The runtime library** — dozens of functions in the top region are Watcom's own
  `strcpy`, `tolower`, `fopen`, and so on. Not game systems, but worth knowing they're
  accounted for. See [game vs library](game-vs-library.md).

## How a map loads (the picture so far)

Instead of matching functions by size, we started following the *call graph* to work
a whole system at once, and the map loader is the first one we mapped. A static scan
of the code for call instructions (`tools/callgraph.py`) shows that the column-table
builder `0x20d18` is called by one function, `0x22858`, a 415-byte routine that is the
map initialiser. Its call tree is the shape of "load and set up a map":

- `0x20d18` — build the column table `g_5358` (offsets become pointers)
- `0x20d98` — a big sibling right after it, almost certainly the block/tile setup
- `0x22768` — reset and index the three object pools (above)
- `0x35ed8` — clear a small 32-entry table
- a cluster of `0x49xxx` helpers — very likely the decompressor, since the game's
  strings include `ERROR decompressing %s` and the map files (`data/map%02d.dat`,
  `data/col01.dat`, `data/hblk01.dat`) are packed

None of this needed a debugger: the game's own strings name every asset file, and the
call graph links the loader to the routines that consume them. That's the method now,
pick a system, find its top function, and match down the tree, understanding each
piece as a part of the whole rather than as an isolated puzzle. The map system is the
one in progress.

## Mission/orders command interpreter — `FUN_00023158` @ 0x23158 (cont. 24 decode)

**TRUE SIZE 5280 (0x14A0)**, 0x23158–0x245f7 (manifest was 107; fixed). Per-record
command interpreter: executes one queued command for record `idx` (u32 param), then
clears the slot. Entry: `ESI = 0x105d4 + idx*0xe` (14-byte command record, opcode at
`record[+0xd]`); `EDX = 0xe49c + idx*0x417` (the 0x417-stride equip/research template
row consumed by matched siblings `FUN_000223c8`/`FUN_00012da8`; `[EDX+0xb5]` =
`g_e551[idx*0x417]` = pool-A base-slot header). Every case ends
`MOV byte [ESI+0xd],0` (consume) → `ADD ESP,0x34` → pops → RET.

Jump table: literal `CS:[EAX*4+0x15920]` → manifest **0x23068** (0x15920+0xd748),
`[table][4B pad @0x23154][code @0x23158]`. **59 dword entries, cases 0x00–0x3a**
(switch = `record[+0xd]`, `CMP AH,0x3a / JA default`). **Default = 0x245ec.**
**41 distinct non-default bodies**; two-bank structure — opcodes 0x01–0x1a mirror
0x21–0x3a (the +0x20 bit selects a variant); 8 pairs share a body outright
(0x01≡0x21, 0x02≡0x22, 0x06≡0x26, 0x0a≡0x2a, 0x12≡0x32, 0x13≡0x33, 0x17≡0x37,
0x18≡0x38), collapsing the work to ~33 unique opcodes.

Case map (case → body):
```
00→DFLT 01→231b7 02→2328d 03→23348 04→234bc 05→23567 06→2363d 07→23655
08→23767 09→23908 0a→239c4 0b→23a28 0c→23cda 0d→23d5e 0e→23de2 0f→23e66
10→23f06 11→243e0 12→24544 13→24592 14→23c10 15→DFLT 16→2447f 17→245dc
18→23303 19→23aaa 1a-20→DFLT
21→231b7 22→2328d 23→233a0 24→23504 25→235c6 26→2363d 27→2369b 28→23823
29→23959 2a→239c4 2b→23a5d 2c→23d0f 2d→23d93 2e→23e17 2f→23ea9 30→24160
31→24421 32→24544 33→24592 34→23c6c 35→DFLT 36→244c8 37→245dc 38→23303
39→23b56 3a→2341e
```
Sampled ops: 1/0x21 = broadcast `FUN_000223c8` (equip-template apply) + `FUN_000229f8`
across players (bound `g_10b0c`), single-player → `FUN_00029d88`, per-player flags to
`g_e4ab`. op 3 = loop pool-A agents (0x8110 stride 0x5c, bound `(g_e551[idx*0x417]+4)`)
calling `FUN_0002f608` (aim/step). op 8 = squad broadcast (`[+0x20]==agent-id` → `FUN_0002f608`).
op 0x16 = `node[0x44]=FUN_00037d08(node,0,cmd[0])` (sub-object spawn, banked 0x37d08).
op 0x39 = per-agent `g_5358` map/tile scan → `node[0x19]=7`, `node[0x58]=7`, clear `+0xa`
bit3. Dominant callee `FUN_0002f608`. **To match: fix size (done), then go body-by-body,
reusing the cont.22 cross-jump law for the recurring merged `FUN_0002f608` call tails.**

## Tactical-map / radar renderer — `FUN_00019608` @ 0x19608 (cont. 25 decode)

**TRUE SIZE 3474 (0x19608–0x1a399; manifest was 1544, fixed).** Full-screen minimap/radar
drawer, `FUN(cam_struct *p1, short zoom)`, frameless + 4 saved regs, 0x644 frame. In the WALLED
g_5358 cluster (reads g_5358 column table + g_10ac0 tile flags + g_810e pool + g_10e grid), so
byte-parity is blocked (g_5358 register wall + IDIV accumulator ties + ~20-slot spill order).
THREE co-located jump tables (lefix rule L+0xd748):
- Table 1 @ 0x19564, 16 entries, index g_10ac0[tile] — terrain-shape polygon draw via 0x3fb40
  (bodies 0x19800/0x19858/0x198b0/0x198ef, default 0x1994a).
- Table 2 @ 0x195a4, 6 entries, index entity type [node+0x18] — blip draw (0x19b29/0x19b61/0x19edd,
  default 0x19f08).
- Table 3 @ 0x195bc, 17 entries, index word [rec+0x1be3e] — HUD/objective markers (0x1a13f/0x1a28c,
  several break to 0x1a38f, default-continue 0x1a384).
Phases: (1) nested 0x60×0x80 tile grid, column lookup + fixed-point corner projection + 0x3fb40
draw; (2) grid-cell entity chains (g_810e+id, 0x12c cap) → type-dispatched blips; (3) two 0x18d18
blip loops + conditional 0x19318 + 8× stride-14 objective-marker records at 0x1be3a. Callees:
0x3fb40/0x3f4b4/0x3f636/0x18d18(×2)/0x19318 (matched: 0x18d18, 0x19318).

## Mission-cursor target-action resolver — `FUN_0002ad58` @ 0x2ad58 (cont. 25 decode)

**TRUE SIZE 3694 (0x2ad58–0x2bbc5; manifest was 1737, fixed). Calls 8 not 4.** Resolves what the
mission cursor points at and writes an action order into `ushort *p` (p[0]=x/id, p[2]=y, p[4]=z,
p[0xd]=action-code; returns int via [esp] slot). Co-located 20-byte jump table at 0x2ad44 (literal
0x1d5fc + 0xd748): `switch(g_e120)` 5 entries → 3 targets (case0→return 0; 1,2→0x2b44c; 3,4,default
→0x2b91e). Sequence: input-mode gates (g_e285/e2a4/e296/e297/e2a3 + g_10b45 → action 2/0x10/0x17),
selection from g_e286-9 into g_e124, already-selected fast path, 4-ped-block shootable-target scan,
move/attack order build (0x2c468 field-copy + 0x1ba48 cursor-line-draw), fresh-target pick via the
R/G/B reticle-ramp interpolators (0x2d7a8/0x2d808) + g_ab60/g_ad60, adjacency/LOS, cursor clamp to
scroll bounds (g_5390/5392/52f8) into reticle window g_10b1c/1e/20, final g_e120 dispatch. DOUBLY
WALLED: indexes the 0x417-stride agent template records (g_e551/e552 via idx=g_10b16) AND the
g_810e pool (0x5c stride) ~12× each with mixed and-form/movsx byte loads; directly calls the parked
register-wall FUN_0002d7a8. Park (decode-only).


## The entity model & the Persuadertron (cont. — systems decode)

Every mobile object — agent, ped, projectile, car — lives in one of **five fixed-size
object pools**, laid out back to back, one pool per class. A single *kind byte* at record
offset `+0x18` tells them apart wherever the shared spatial grid is walked, and the pool
record-counts line up exactly with the game's level-data arrays:

| pool | base | count | class | kind `[0x18]` |
|------|------|------:|-------|------|
| A | 0x8110 | 256 | people (agents / peds / projectiles) | 1, 2 |
| B | 0xdd10 | 64 | **cars / vehicles** | 5 |
| C | 0xe790 | 400 | statics | — |
| D | 0x11670 | 512 | weapons / pickups | 4 |
| E | 0x15e70 | 256 | sfx / bullets | 3 |

Each pool-A entity runs a **behaviour** selected by its state byte through a jump table
(`entity_behaviour_dispatch`, 0x2ea88), driven once per frame by the pool tick (0x31858).
The **Persuadertron** is one of those behaviours: `persuade_capture` (0x2fe68). On contact
with the target ped it sets the ped's *leader link* to the agent, chains the ped into the
agent's follower group, raises a "controlled" flag, and clamps the ped's amount by the
per-type maximum-quantity table `g_item_max_qty` (0xa73a). The converted ped then follows
the agent. Allegiance is otherwise positional (team = pool index & ~7); a persuaded ped is
recognised as friendly by that flag plus its leader link. Full field-level detail is in
[the object model](object-model).

## Vehicles / cars — pool B

Cars are pool B (kind 5). `vehicle_hp_stamp` (0x20d98) stamps each car's hit points from
its model byte when a map loads — this confirms the long-standing guess that pool B held
"typed HP objects": they are the vehicles. `vehicle_pool_tick` (0x36fd8) redraws every car
each frame and, if the car is anchored to a pool-A entity, places its body at that
entity's position plus an offset (so a driven car's body follows the driver).

Getting in and out is a small state machine operating on a **doubly-linked occupant list**
hung off the car: `vehicle_board` (0x2fa48) links an agent in and inherits the car's speed
(hiding the rider for trains and boats); `vehicle_ride` (0x2fca8) slaves each passenger's
position to the car every frame; `vehicle_exit` (0x2fbc8) unlinks and drops the agent
beside it. Driving itself is `vehicle_drive_step` (0x34858): a real speed model that
accelerates, brakes into corners, and **steers by following directional road tiles** —
a road tile's value (6/7/8/9) encodes which way traffic flows through it. *(These four
handlers were previously mis-labelled `weapon_fire` / `formation_follow` /
`join_new_leader` / `detach_entity_type`; the byte matches were fine, the names were
wrong, and they are corrected.)*

## Where the game's data actually lives — RNC resource loading

A recurring surprise: many of the tables the game reads (weapon quantities, direction
vectors, the equipment database) are **zero in the shipped executable** and filled at
runtime. The stats were never compiled in. They live in external `data/*.dat` files —
whose names sit in a table in OBJECT2, next to the multilingual equipment descriptions —
loaded by a descriptor-driven resource loader:

- `validate_records_or_abort` (0x18338) walks a list of block descriptors and aborts with
  an error if any load fails.
- `realloc_block_descriptor` (0x184b8) loads one block: `'*'` descriptors just allocate a
  zeroed block; otherwise it opens the named file, sizes it (RNC-aware), reads it, and
  decompresses in place if the file was packed.
- `rnc_decompress` (0x3a1ec) is **Rob Northen Compression, method 1** — magic `RNC\x01`,
  big-endian sizes, Huffman tables plus an LZ back-reference copy. The classic packer of
  the era; recognising it is a reusable win for other Bullfrog / DOS-game decomps (noted
  in the [porting guide](porting-guide)).

This is the clean answer to "is OBJECT1 the whole game": the *logic* is all here; the
*data* (art, sound, and the balance numbers) is external and RNC-packed, which is exactly
why a stat table can read as all-zeros with nothing actually missing.
