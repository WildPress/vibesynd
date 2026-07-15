# The object model & Bullfrog coding style (derived from matched bytes)

This page is **evidence-based**: everything here is inferred from the ~85 functions we've
matched byte-for-byte. Because a relocation-exact match forces our C to mirror the original
source's structure, the recurring shapes below are effectively Bullfrog's own conventions,
not our guesses about them. Use this when writing a new (especially complex) function: name
and type the fields from here so the first compile is close.

## The memory model

Global state lives at **fixed absolute addresses** (there is almost no `malloc` in the hot
path — objects live in static pools). Three contiguous fixed-record pools, back to back
(see `game-systems.md`, fn `0x22768`):

| pool | base | records | stride | "in use" flag |
|------|------|---------|--------|---------------|
| A ("entities") | `0x8110` | 256 | 92 (`0x5c`) | byte `+0x18` |
| B ("typed HP objects") | `0xdd10` | 64 | 42 (`0x2a`) | byte `+0x18` |
| C | `0xe790` | 400 | 30 (`0x1e`) | byte `+0x18` |

**The key idiom — links are 16-bit ids, not pointers.** Everywhere we walk a list we do
`node = g_810e + id`, where `g_810e = 0x810e = poolA_base - 2` and `id` is a *byte offset*
into the pool (so `id` of record k is `2 + k*0x5c`). A node's "next"/"prev" fields store the
next node's `id`, and `id == 0` terminates. `pid = (unsigned short)(p - g_810e)` recovers an
id from a pointer. This id-instead-of-pointer scheme (half the size, position-independent,
survives a save/load) is the hallmark early-90s / Amiga-derived Bullfrog style, and it's the
single most useful thing to recognise. Declare the pool as `extern unsigned char g_810e[];`.

## Pool A record — the central "entity" (person / agent / projectile)

Field offsets and widths are exactly as they appear in matched code; meanings are inferred
from how the matched functions use them (evidence in parentheses). Confidence varies — the
high-frequency fields are certain, the rare ones are educated.

| off | width | meaning (inferred) | evidence |
|-----|-------|--------------------|----------|
| `0x00` | u16 | link: **next** id (grid / chain) | 0x26e18 head-insert, 0x26da8 unlink |
| `0x02` | u16 | link: **prev** id | 0x26e18/0x26da8 (`p[2]`) |
| `0x04` | s16 | world **X** | 0x2f608/0x2d738 aim/step from `obj[4]` |
| `0x06` | s16 | world **Y** | `obj[6]` |
| `0x08` | s16 | world **Z**; high byte = **level** | 0x34088 (`node[8]` hi byte == level) |
| `0x0a` | u8 | **flags A** (bit2 = "in grid", bit3 set on fire) | 0x26e18 `&4`, 0x2f608 `|=8` |
| `0x0b` | u8 | **flags B** (bit0) | 0x34088 `node[0xb]&1` |
| `0x0c` | ptr | handle / sub-object pointer | 0x146f8/0x13bc8 `obj[0xc]` |
| `0x10` | u16 | index into 8-byte-record table `g_5338` | 0x269d8 (advance a linked index) |
| `0x14` | u16 | **hit points** (stamped from type at load) | 0x20d98 HP table (pool B) |
| `0x18` | u8 | **type code** / "in use" flag | 0x34088/0x2e588 (`==1`/`==2`) |
| `0x19` | u8 | **sub-type / animation frame** (very hot) | 19 uses; 0x37738 `g_a6fe[node[0x19]]` |
| `0x1a` | u8 | **facing** (0..255 direction) | 0x2f608 sets `obj[0x1a]` from atan2 |
| `0x1b` | u8 | secondary angle | 0x2f608 `obj[0x1b]` |
| `0x1c` | u16 | link: **next** id (a second chain) | 9 uses; 0x13bc8/0x34118 walk |
| `0x24` | u16 | link: list **head** | 0x2e588 |
| `0x3a` | u16 | id into another list | 0x13bc8/0x13b38 |
| `0x44` | u16 | linked id → node whose `[0x19]` indexes a sprite table | 0x37738/0x37818 |
| `0x46` | u8 | sprite/frame byte (`= g_a6fe[...]`) | 0x37738 |
| `0x48/0x49` | u8,u8 | a (min,max) byte pair | 0x2d868 |
| `0x4c/0x4d` | u8,u8 | a (min,max) byte pair | 0x2d808 |
| `0x50/0x51` | u8,u8 | a (min,max) byte pair | 0x2d7a8 |
| `0x54` | u8 | **current health / value** (hottest field) | 20 uses; 0x34118 `[0x54]-=dmg` |
| `0x55` | u8 | **speed / max** (paired with 0x54) | 0x2d6c8/0x34118 |
| `0x58` | — | ? (7 uses) | — |

The three consecutive `(min,max)` byte pairs at `0x48/0x4c/0x50`, each fed through the same
`x*delta/(hi-lo)` interpolation (fns 0x2d7a8/0x2d808/0x2d868), look like **three lighting or
colour channels** (R/G/B ramps) — a strong guess for the next function in that cluster.

## Global data catalogue (what the fixed addresses hold)

Named globals carry a semantic name in `src/` (the address is the anchor, kept in
`manifest/globals.json`; `mkdata.py` resolves name → address registry-first so the data
image is unaffected). Rename more with `tools/rename_global.py`.

| symbol (addr) | type | role |
|--------|------|------|
| `g_entity_pool` (0x810e) | `u8[]` | pool A base − 2 (entity pool; index by id) |
| `g_grid_heads` (0x10e) | `u16[]` | 128×128 spatial-grid head table (cell → first entity id) |
| `g_dir_dx`, `g_dir_dy` (0xab60/0xad60) | `s16[256]` | direction → (dx, dy) vectors (256-step angle) |
| `g_screen_buf`, `g_back_buf` (0x5368/0x5370) | `u8*` | the two screen/back buffers (blit targets) |
| `g_rec8_table` (0x5338) | `u8*` | table of 8-byte records (linked via `+6`, flag `+5`) |
| `g_objective_slots` (0x5350) | `u8*` | a `0x20`-stride record table (map/objective slots) |
| `g_map_cols` (0x5358) | `int*` | map column-pointer table (built by 0x20d18) |
| `g_shot_level/y/x` (0x10b5a/c/e) | s16 | shot cursor: level / y / x accumulators (collision walk) |
| `g_cursor_x/y` (0x10b22/24) | u16 | a cursor point (x, y) tested against record boxes |
| `g_minimap` (0xdb2c) | `u8[]` | 0x19×0x10 minimap grid |
| `g_11bec/g_11bed` | `u8[]` | paired flag/value tables (0x35ed8 clears, 0x35f28 writes) — *unnamed* |
| `g_a6fe`, `g_a686`, `g_b46a` | `u8[]` | byte translation / palette tables — *unnamed* |

## Record-table strides (fixed-stride arrays; index by `base + i*stride`)

The game stores most collections as flat fixed-stride record arrays. Recognising the
stride tells you the record boundary (and often the record type). Catalogued from
matched code + the named globals:

| stride | table(s) | notes |
|--------|----------|-------|
| `0x417` (1047) | `g_player_recs` (0xe49c), `g_player_budget` (+4), `g_agent_slots` (0xe551), `g_agent_tmpl` (0xe552) | **per-player** record; the e4xx/e5xx globals are field-views into it (index `i*0x417`) |
| `0x5c` | `g_pool_a` (0x8110) | entity/agent pool slot (`g_entity_pool` = pool−2 for id-indexing) |
| `0x1eb` (491) | `g_list_recs` (0x5780), research records | byte+0 = type (0xff terminator), word+8 = state |
| `0x1f5` (501) | mod records, conveyor rows (`g_7bf4`) | *aliased* — two subsystems read the same base at different offsets |
| `0xe` (14) | `g_command_recs` (0x105d4, 8 recs); `g_objectives` (0x1be3a, 8 slots); `g_player_status` (0x105e1) | short fixed lists |
| `0xa` (10) | `g_syndicate_recs` (0x539c, 50 recs) | |
| `0x12` (18) | `g_hud_panel` (0x5114), `g_auxbar_panel` (0x5258) | UI panel slots: x@+0, y@+2 |
| `8` | `g_rec8_table` (0x5338) | linked via +6, flag byte +5 |
| `5` | `g_rec5_table` (0x5340) | linked; `base + id*5` |

## Coding-style conventions (for writing matchable C)

Patterns that recur across the corpus — follow them and the first compile lands close:

- **Field access is raw pointer + cast**, not structs: `*(short *)(p + 0x54)`, `p[0x19]`.
  (The original almost certainly used structs, but struct *indexing* changes Watcom's
  addressing — see 0x269d8 — so for matching we keep casts and just document meaning here.)
- **Signedness by field kind:** coordinates are `short` (signed); health / type / flags /
  frame are `unsigned char`; ids and links are `unsigned short`; the odd table value is
  `unsigned short` (zero-extend load, but `/2` stays signed via int promotion — see 0x263f8).
- **State is global, not threaded through params.** Many functions take 0–2 params and read a
  wall of `g_*`. When a param's stack offset is too high, suspect decompiler-invented params.
- **Return types are narrow:** `char`/`unsigned char` for a 0/1 result (just sets AL);
  `unsigned short` when the result is used as 16-bit (`xor ah,ah` vs `and eax,0xff`).
- **Loop idioms:** `do { ... } while (cond)`; unrolled `*dst++ = *src++` copies; stores use
  the *pre-increment* value; counted loops compare with `!=`/`<` on 16-bit registers.
- **Everything is table-driven:** direction vectors, HP-by-type, tile-by-type, palette ramps.
  A new function is usually "index a table by a byte field, act on the result".

## Is this "the Bullfrog style"?

What the bytes show lines up with the well-known shape of early-90s Bullfrog / Amiga-heritage
engines (Populous → Syndicate → Dungeon Keeper): static object pools indexed by small ids,
intrusive 16-bit-id linked lists, a spatial grid, single-byte packed flags and stats, 256-step
byte angles with sin/cos-style vector tables, and global fixed-address state. We can't cite an
internal style guide, but the corpus is internally consistent enough that these conventions are
safe to assume for functions we haven't seen yet — which is the point of writing them down.
