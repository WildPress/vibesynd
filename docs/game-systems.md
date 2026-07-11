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
