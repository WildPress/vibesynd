# The object model & Bullfrog coding style (derived from matched bytes)

Everything here is inferred from the ~85 functions we've matched byte-for-byte. Because a
relocation-exact match forces our C to mirror the original source's structure, the recurring
shapes below are effectively Bullfrog's own conventions, not our guesses about them.

Use this when writing a new function, especially a complex one. Name and type the fields from
here so the first compile is close.

## The memory model

Global state lives at **fixed absolute addresses**. There's almost no `malloc` in the hot
path, objects live in static pools. Three contiguous fixed-record pools, back to back
(see `game-systems.md`, fn `0x22768`):

| pool | base | records | stride | "in use" flag |
|------|------|---------|--------|---------------|
| A ("entities") | `0x8110` | 256 | 92 (`0x5c`) | byte `+0x18` |
| B ("typed HP objects") | `0xdd10` | 64 | 42 (`0x2a`) | byte `+0x18` |
| C | `0xe790` | 400 | 30 (`0x1e`) | byte `+0x18` |

**The key idiom is that links are 16-bit ids, not pointers.** Everywhere we walk a list we do
`node = g_810e + id`, where `g_810e = 0x810e = poolA_base - 2` and `id` is a *byte offset*
into the pool (so `id` of record k is `2 + k*0x5c`). A node's "next"/"prev" fields store the
next node's `id`, and `id == 0` terminates. `pid = (unsigned short)(p - g_810e)` recovers an
id from a pointer. This id-instead-of-pointer scheme (half the size, position-independent,
survives a save/load) is the hallmark early-90s / Amiga-derived Bullfrog style, and it's the
single most useful thing to recognise. Declare the pool as `extern unsigned char g_810e[];`.

## Pool A record: the central "entity" (person / agent / projectile)

Field offsets and widths are exactly as they appear in matched code. Meanings are inferred
from how the matched functions use them, with evidence in parentheses. Confidence varies. The
high-frequency fields are certain, the rare ones are educated guesses.

| off | width | meaning (inferred) | evidence |
|-----|-------|--------------------|----------|
| `0x00` | u16 | link: **next** id (grid / chain) | 0x26e18 head-insert, 0x26da8 unlink |
| `0x02` | u16 | link: **prev** id | 0x26e18/0x26da8 (`p[2]`) |
| `0x04` | s16 | world **X** | 0x2f608/0x2d738 aim/step from `obj[4]` |
| `0x06` | s16 | world **Y** | `obj[6]` |
| `0x08` | s16 | world **Z**, high byte = **level** | 0x34088 (`node[8]` hi byte == level) |
| `0x0a` | u8 | **flags A** (bit2 = "in grid", bit3 set on fire) | 0x26e18 `&4`, 0x2f608 `|=8` |
| `0x0b` | u8 | **flags B** (bit0) | 0x34088 `node[0xb]&1` |
| `0x0c` | ptr | handle / sub-object pointer | 0x146f8/0x13bc8 `obj[0xc]` |
| `0x10` | u16 | index into 8-byte-record table `g_5338` | 0x269d8 (advance a linked index) |
| `0x14` | u16 | **hit points** (stamped from type at load) | 0x20d98 HP table (pool B) |
| `0x18` | u8 | **type code** / "in use" flag | 0x34088/0x2e588 (`==1`/`==2`) |
| `0x19` | u8 | **sub-type / animation frame** (very hot) | 19 uses, 0x37738 `g_a6fe[node[0x19]]` |
| `0x1a` | u8 | **facing** (0..255 direction) | 0x2f608 sets `obj[0x1a]` from atan2 |
| `0x1b` | u8 | secondary angle | 0x2f608 `obj[0x1b]` |
| `0x1c` | u16 | link: **next** id (a second chain) | 9 uses, 0x13bc8/0x34118 walk |
| `0x1d` | u8 | **flags C** (bit1, bit2) | 20 uses, `n[0x1d]\|=2`, `&4` test |
| `0x24` | u16 | link: list **head** | 0x2e588 |
| `0x3a` | u16 | id into another list | 0x13bc8/0x13b38 |
| `0x44` | u16 | linked id → node whose `[0x19]` indexes a sprite table | 0x37738/0x37818 |
| `0x46` | u8 | sprite/frame byte (`= g_a6fe[...]`) | 0x37738 |
| `0x48/0x49` | u8,u8 | a (min,max) byte pair | 0x2d868 |
| `0x4c/0x4d` | u8,u8 | a (min,max) byte pair | 0x2d808 |
| `0x50/0x51` | u8,u8 | a (min,max) byte pair | 0x2d7a8 |
| `0x54` | u8 | **current health / value** (hottest field) | 20 uses, 0x34118 `[0x54]-=dmg` |
| `0x55` | u8 | **speed / max** (paired with 0x54) | 0x2d6c8/0x34118 |
| `0x58` | u8 | **state code** (recomputed, then mirrored to 0x19 anim) | `recompute_state_code` (0x2d998), set 0x1c/0x1f/0x20 |
| `0x5a` | u8 | value stepped by ±0x40/±0x80 (angle/scroll?) | `obj[0x5a]±0x40`, 18 uses |

The three consecutive `(min,max)` byte pairs at `0x48/0x4c/0x50`, each fed through the same
`x*delta/(hi-lo)` interpolation (fns 0x2d7a8/0x2d808/0x2d868), look like **three lighting or
colour channels** (R/G/B ramps), a strong guess for the next function in that cluster.

## Global data catalogue (what the fixed addresses hold)

Named globals carry a semantic name in `src/`. The address is the anchor, kept in
`manifest/globals.json`, and `mkdata.py` resolves name → address registry-first so the data
image is unaffected. Rename more with `tools/rename_global.py`.

| symbol (addr) | type | role |
|--------|------|------|
| `g_entity_pool` (0x810e) | `u8[]` | pool A base − 2 (entity pool, index by id) |
| `g_grid_heads` (0x10e) | `u16[]` | 128×128 spatial-grid head table (cell → first entity id) |
| `g_dir_dx`, `g_dir_dy` (0xab60/0xad60) | `s16[256]` | direction → (dx, dy) vectors (256-step angle) |
| `g_screen_buf`, `g_back_buf` (0x5368/0x5370) | `u8*` | the two screen/back buffers (blit targets) |
| `g_rec8_table` (0x5338) | `u8*` | table of 8-byte records (linked via `+6`, flag `+5`) |
| `g_objective_slots` (0x5350) | `u8*` | a `0x20`-stride record table (map/objective slots) |
| `g_map_cols` (0x5358) | `int*` | map column-pointer table (built by 0x20d18) |
| `g_shot_level/y/x` (0x10b5a/c/e) | s16 | shot cursor: level / y / x accumulators (collision walk) |
| `g_cursor_x/y` (0x10b22/24) | u16 | a cursor point (x, y) tested against record boxes |
| `g_minimap` (0xdb2c) | `u8[]` | 0x19×0x10 minimap grid |
| `g_11bec/g_11bed` | `u8[]` | paired flag/value tables (0x35ed8 clears, 0x35f28 writes), *unnamed* |
| `g_a6fe`, `g_a686`, `g_b46a` | `u8[]` | byte translation / palette tables, *unnamed* |

## Record-table strides (fixed-stride arrays, index by `base + i*stride`)

The game stores most collections as flat fixed-stride record arrays. Recognising the
stride tells you the record boundary (and often the record type). Catalogued from
matched code + the named globals:

| stride | table(s) | notes |
|--------|----------|-------|
| `0x417` (1047) | `g_player_recs` (0xe49c), `g_player_budget` (+4), `g_agent_slots` (0xe551), `g_agent_tmpl` (0xe552) | **per-player** record, the e4xx/e5xx globals are field-views into it (index `i*0x417`) |
| `0x5c` | `g_pool_a` (0x8110) | entity/agent pool slot (`g_entity_pool` = pool−2 for id-indexing) |
| `0x1eb` (491) | `g_list_recs` (0x5780), research records | byte+0 = type (0xff terminator), word+8 = state |
| `0x1f5` (501) | mod records, conveyor rows (`g_7bf4`) | *aliased*, two subsystems read the same base at different offsets |
| `0xe` (14) | `g_command_recs` (0x105d4, 8 recs), `g_objectives` (0x1be3a, 8 slots), `g_player_status` (0x105e1) | short fixed lists |
| `0xa` (10) | `g_syndicate_recs` (0x539c, 50 recs) | |
| `0x12` (18) | `g_hud_panel` (0x5114), `g_auxbar_panel` (0x5258) | UI panel slots: x@+0, y@+2 |
| `8` | `g_rec8_table` (0x5338) | linked via +6, flag byte +5 |
| `5` | `g_rec5_table` (0x5340) | linked, `base + id*5` |

## The per-player record (`g_player_recs`, stride 0x417)

One `0x417`-byte record per player, based at `g_player_recs` (0xe49c), indexed
`g_player_recs + player*0x417`. Almost every `g_e4xx`/`g_e5xx` global is a *field-view*
into this record (its address = `0xe49c + field-offset`), so `g_field[player*0x417 + …]`
reads that field for a given player. Fields mapped from matched code:

| offset | field-view global | type | meaning |
|--------|-------------------|------|---------|
| +0x0   | `g_player_recs`   | n/a | record base / accumulators |
| +0x4   | `g_player_budget` | int  | money/budget |
| +0xe   | `g_player_flags`  | u8   | flags, **bit 2 = active/remote** player (triggers equip build 0x12da8) |
| +0xf   | `g_player_owner`  | u8   | owner / actor id (`(pool_a_ptr − 0x8110)/0x5c/8`) |
| +0xb5  | `g_agent_slots`   | u8[] | agent-slot lookup (base agent indices into `g_pool_a`) |
| +0xb6  | `g_agent_tmpl`    | u8[] | agent template bytes |
| +0x11d | **squad/equip grid** | n/a | array of **40-byte rows** (squad member `j*40`), see below |
| +0x40b | `g_e8a7`          | u8   | template-valid flag (set 1 after build), *unnamed* |

**Squad/equip grid** (each 40-byte row, member `j`):

| row offset | field-view | type | meaning |
|------------|-----------|------|---------|
| +0x0 | `g_squad_id`   | u8   | member id/type (0xff = empty slot) |
| +0x1 | `g_e5ba`       | u16  | value, 0x10 default / 0xffff empty, *unnamed* |
| +0x3 | `g_e5bc`       | u16  | *unnamed* |
| +0x7 | `g_squad_slot` | u8   | slot index (`i+1`) |
| +0x8 | equip sub-array | n/a | per weapon/mod slot (`d*4`): `g_equip_qty` (u16) + `g_equip_kind` (u16) |

The remaining low-offset field-views (`g_e4a4/a6/a8/ac/ad/bf`, `g_e553/555/587`) are
narrow single-use fields left unnamed.

## Coding-style conventions (for writing matchable C)

Patterns that recur across the corpus. Follow them and the first compile lands close:

- **Field access is raw pointer + cast**, not structs: `*(short *)(p + 0x54)`, `p[0x19]`.
  (The original most likely used structs, but struct *indexing* changes Watcom's
  addressing, see 0x269d8, so for matching we keep casts and just document meaning here.)
- **Signedness by field kind.** Coordinates are `short` (signed), health / type / flags /
  frame are `unsigned char`, ids and links are `unsigned short`, and the odd table value is
  `unsigned short` (zero-extend load, but `/2` stays signed via int promotion, see 0x263f8).
- **State is global, not threaded through params.** Many functions take 0–2 params and read a
  wall of `g_*`. When a param's stack offset is too high, suspect decompiler-invented params.
- **Return types are narrow.** `char`/`unsigned char` for a 0/1 result (just sets AL),
  `unsigned short` when the result is used as 16-bit (`xor ah,ah` vs `and eax,0xff`).
- **Loop idioms.** `do { ... } while (cond)`, unrolled `*dst++ = *src++` copies, stores use
  the *pre-increment* value, and counted loops compare with `!=`/`<` on 16-bit registers.
- **Everything is table-driven.** Direction vectors, HP-by-type, tile-by-type, palette ramps.
  A new function is usually "index a table by a byte field, act on the result".

## Is this "the Bullfrog style"?

What the bytes show lines up with the well-known shape of early-90s Bullfrog / Amiga-heritage
engines (Populous → Syndicate → Dungeon Keeper): static object pools indexed by small ids,
intrusive 16-bit-id linked lists, a spatial grid, single-byte packed flags and stats, 256-step
byte angles with sin/cos-style vector tables, and global fixed-address state. We can't cite an
internal style guide, but the corpus is internally consistent enough that these conventions are
safe to assume for functions we haven't seen yet, which is the point of writing them down.

## The Persuadertron (traced end-to-end via the LE-relocation resolver)

The persuade weapon is a per-state entity **behaviour**. Every live pool-A entity runs a behaviour
selected by its state byte `[0x19]` through the dispatcher **`entity_behaviour_dispatch` (0x2ea88)**,
a jump table over ~45 states. The per-frame loop **`entity_pool_tick` (0x31858)** calls it for
each entity. (Correction: 0x31858 is the loop, not the switch. The switch is 0x2ea88.)

- **`persuade_capture` (0x2fe68)** is the convert step. The agent seeks the target ped. On contact
  (agent x/y == ped x/y, |z diff| < 0x81), if the ped isn't already taken (`ped[0x0a] & 1 == 0`):
  1. clamp the ped's amount `[0x14]` to **`g_item_max_qty[ped_type]`** (0xa73a), the per-type
     max-quantity table (also used to stock equipment ammo), not a persuade-specific limit.
  2. append the ped into the agent's **follower chain** (`agent[0x3a]` head, `[0x1c]`/+0x812a links).
  3. `ped[0x0a] |= 1`, set the **persuaded/controlled** flag.
  4. `ped[0x20] = agent`, set the ped's **leader link** to the agent (allegiance flipped).
- If not yet in contact, `persuade_capture` sets the ped's state `[0x19]=5` (seek/approach). This is
  a behaviour-state index, **not a "weapon type."** (An earlier version of this note mis-attributed
  it to a `weapon_fire` function at 0x34858 with a "guided type 5" mode. That was wrong. 0x34858 is
  the **vehicle-drive step**, see the Vehicle system section. The persuade_capture mechanism above
  is unaffected and verified.)
- The converted ped then runs the squad **`follow_leader`** behaviour (0x30078), reading its `[0x20]`
  leader link. The squad follower chain (`leader[0x3a]` head, `[0x1c]` next, `[0x1e]` prev, `[0x20]`
  leader) is a **different structure** from the vehicle occupant list, see Vehicle system.

Allegiance is otherwise positional (a ped's team = poolIndex & ~7). The persuaded ped is
recognised as friendly via the `[0x0a]` bit-0 flag + its leader link.

## The vehicle / car system (traced via a 3-agent sweep, cross-verified)

### Five object pools, one per class: discriminated by the kind byte `[0x18]`
The engine has five back-to-back fixed-record pools. Their record counts lock onto Syndicate's
level-data arrays, and a single kind byte at `+0x18` tells them apart wherever the shared spatial
grid (`g_10e`) is walked:

| pool | base | count | stride | class | `[0x18]` kind |
|------|------|------:|-------|-------|-----|
| A | 0x8110 | 256 | 0x5c | people (agents / peds / projectiles) | 1, 2 |
| B | 0xdd10 | 64 | 0x2a | **cars / vehicle bodies** | 5 |
| C | 0xe790 | 400 | 0x1e | statics | (unobserved) |
| D | 0x11670 | 512 | 0x24 | weapons / pickups | 4 |
| E | 0x15e70 | 256 | 0x1e | sfx / bullets | 3 |

Pool bounds come from `FUN_00022768`. Kind immediates are stored by the spawners
(`spawn_pool_11670`=4, `spawn_pool_15e70`=3). Kinds 1/2 (pool A) are loaded from the mission
descriptor, never stored as immediates.

### Car bodies: pool B (0xdd10, kind 5)
- `vehicle_hp_stamp` (0x20d98, from `mission_map_init`) stamps each car's HP `[0x14]` by model byte
  `[0x19]`: models 1/2→600, 5/6→100, 9/10→80, 13/14→30, 17/18→40, 28/29→10, 36/37→120, 40/41→115.
- `vehicle_pool_tick` (0x36fd8) runs each frame over pool B: a car with anchor `[0x20]==0` sits at
  its own X/Y/Z. With `[0x20]!=0`, `[0x20]` is a pool-A entity id and the body is placed at that
  entity's position + offset `[0x22]/[0x24]/[0x26]` (re-linked into the grid via `move_entity_xyz`),
  then drawn by model `[0x19]` (sprite jump table at data 0xd390).

Body fields: `[0x14]` HP, `[0x18]`=5, `[0x19]` model, `[0x20]` anchor pool-A id, `[0x22]/[0x24]/[0x26]` offset.

### Occupancy + driving: pool-A ride handlers (behaviour states, no static xrefs)
A ped boards/drives through pool-A behaviour states operating on a **tag-2 pool-A entity** (the
ridden vehicle for movement purposes, the pool-B body follows it via the anchor). The occupant list
is separate from the squad follower chain:

- **`vehicle_board` (0x2fa48)**. On reaching the target vehicle `[0x2c]`, link self into the
  occupant list: empty → `vehicle[0x1c]=self`, else walk `[0x22]` forward-links to the tail and
  append. Set taken flag `[0x0a]|=1`. Subtype `[0x19]` 9/0xa (train/boat) also hides (`|=8`). Copy
  vehicle max-speed `[0x28]`→self `[0x55]`, then snap to the vehicle.
- **`vehicle_drive_step` (0x34858)**. Called only from the driver states `vehicle_drive_state`
  (0x2f878) / `vehicle_move_drive` (0x2f908). Accel/brake `[0x54]`→`[0x55]` by turn sharpness
  (stop / hard-brake / half / +4-cap), steer by directional road tiles, stop off-road, and commit
  both vehicle and rider via `move_entity_xyz`. **(This is the function previously mis-named
  `weapon_fire`.)**
- **`vehicle_ride` (0x2fca8)**. Passenger carry: walk `[0x24]` back-links to the type-2 vehicle,
  copy its X/Y/Z into the passenger each frame. (Previously mis-named `formation_follow`.)
- **`vehicle_exit` (0x2fbc8)**. Doubly-linked removal from the occupant list, restore speed, clear
  taken flag, place one tile beside the vehicle. Reached from dismount triggers 0x36c28/0x36c78.
  (Previously mis-named `detach_entity_type`.)

Occupant-list fields: vehicle `[0x1c]` = list head, occupant `[0x22]` = forward link, `[0x24]` = back
link (head's back → the vehicle). Distinct from the squad chain (`[0x3a]/[0x1e]/[0x20]`).

### Road following
Roads are **directional flow tiles**: 6=W, 7=E, 8=N, 9=S. `compass_tile_probe` (0x34368) returns 1
only if the tile in a given direction carries the matching flow code. The drive step steers to stay
on valid road tiles and looks several tiles ahead (`FUN_00034198`) for corners and traffic (via the
cell probe `shot_collision_query` 0x34088, which returns a vehicle/ped directly ahead).

### Confirmed vs inferred
**Confirmed** (first-hand or strong evidence): the five-pool model + counts, cars = pool B kind 5,
the HP-stamp and pool-B tick, and the board/ride/drive/exit handler mechanics and occupant fields (the
train/boat-hide, road-tile steering, and vehicle-speed inheritance are unambiguous vehicle markers).
**Inferred / not fully pinned:** whether the tag-2 pool-A entity the ride handlers touch is the car
itself, a driver/mount actor, or shares representation with a squad leader. Settling that needs the
pool-A loader that assigns kind 1 vs 2. Road-tile codes are from drive-helper analysis, not yet
byte-verified against a running map.

## Static image vs. runtime-filled data (verified from OBJECT2 bytes)

OBJECT2 (DGROUP `[0, 0xd000)`) is a *mix* of genuinely-initialised data (nonzero bytes,
e.g. from 0x3e1f) and regions that are **zero in the shipped image and populated at runtime**.
Do not infer "static content" from the address range alone. Check the bytes.

Verified zero-in-image, runtime-filled tables (read-only from code, no literal-address writes):
- `g_item_max_qty` (0xa73a), per-item-type max quantity. **15 reads, 0 literal writes**, bytes zero.
- `g_dir_dx`/`g_dir_dy` (0xab60/0xad60), direction sin/cos vectors. Bytes zero.

For "is OBJECT1 the whole game", the *logic* is complete in code, but the game's
**balance data** (weapon quantities/stats, direction tables) is loaded into DGROUP at runtime
by a bulk copy, not baked in as code literals. A table being zero here is expected, not missing.

## OBJECT2 contents map (verified by scanning the raw bytes)

mkdata emits OBJECT2 at data-image base 0x0 (`emit_ledata(0x0, d2, ...)`), so **data address ==
OBJECT2 file offset** (direct). 20% of OBJECT2 is nonzero, the rest is zero regions that are
runtime-filled working state. Observed layout:

| addr range | content |
|-----------|---------|
| `0x0` .. ~`0x2800` | **equipment/cyberware descriptions**, 3 languages (EN/FR/IT), "LEGS V1", night-vision, "NEURAL ENHANCER", "CEREBRAL MULTIPLIER", etc. (indexed string block) |
| ~`0x2c00` .. `0x7c00` | sparse: scattered small tables + more strings |
| `0x8100` | static **u16 ramp table** (`0007 0007 0007 ... 0008 0008 ...`), a level/count -> value lookup |
| `0x8400` | static **(value, 0xff) scaling pairs** (`1fff 1cff 19ff ...`), attenuation/interp ramp |
| `0x90b8` | `c:/synd/save` save path, then the **data-file name table**: `data/mspr-0.dat`, `data/mfnt-0.dat`, `data/mlogos.dat`, `data/col01.dat`, `data/mselect.pal`, `data/sound-1.dat`, ... |
| `0x9800` .. `0xd000` | mostly zero (incl. `g_item_max_qty` 0xa73a, campaign-init buffers 0xb474/0xb498/0xb830) |

**Weapon/equipment data split (traced):** the item *text* roster is static in OBJECT2, the
*numeric* tables are runtime-filled. `new_campaign_reset` (0x20fc8) builds a campaign's working
tables by copying the research header from `0xb474` -> `0x5788` (stride 0x1eb, 18 entries) and the
equipment header from `0xb498` -> `0x7c05` (stride 0x1f5, 20 entries), plus a per-item byte from
`0xb830`. A cheat flag (g_10b43) instead sets every entry to 0x960 (all-unlocked). Those source
buffers have exactly ONE reference each (the read here) and no literal-address writer, so they are
populated by a register-indirect bulk load earlier in startup (inferred: from a `data/*.dat` file
via the name table above). The fixed per-type capacities `g_item_max_qty` (0xa73a) are likewise
zero-in-image and read-only from code, so they're loaded at runtime, not baked in.

## Resource loading architecture (traced end-to-end)

How external data reaches the runtime tables. The loader is descriptor-driven:

1. `validate_records_or_abort` (0x18338) walks a linked list of **block descriptors**
   (stride 0x2c: filename/`'*'` @+0, block-ptr-ptr @+0x1c, end-ptr-ptr @+0x20, size @+0x24,
   flags @+0x28 bit0 = DPMI-alloc vs heap, DPMI selector @+0x2a, next @+0x48). It calls the
   loader per entry and aborts with an error string if any fail.
2. `realloc_block_descriptor` (0x184b8) loads one block:
   - first byte `'*'` just allocates a zero-filled block (no file).
   - otherwise the descriptor names a **file**: open (`cond_3call` 0x18828, mode 0x200),
     get the RNC-aware unpacked size (`open_detect_rnc_header` 0x18958), allocate, read
     (`file_read_n` 0x188a8). If the file was compressed, decompress in place.
3. `rnc_decompress` (0x3a1ec) is **Rob Northen Compression method 1**. Magic check
   `*p==0x4e52 && p[1]==0x143` ("RNC"), big-endian sizes via `rnc_read_be_len` (0x3a37a),
   Huffman tables built by `rnc_make_huffman` (0x3a449) at 0xbe30/0xbeb0/0xbf30, symbols read
   by `rnc_read_huffman` (0x3a383) over the bit stream from `rnc_input_bits` (0x3a3c6), then the
   LZ back-reference copy (distance+1, length+2).

So the game's data files (`data/*.dat`, name table at OBJECT2:0x90b8) are RNC-compressed. The
weapon/equipment stat tables (`g_item_max_qty` 0xa73a, campaign-init headers 0xb474/0xb498) get
their values from these decompressed blocks, copied into the fixed working tables during
campaign/mission init (`new_campaign_reset` 0x20fc8, etc.).
