# `src/` — Syndicate reconstruction, organized by subsystem

This is a **matching decompilation**: every file is one original function, reconstructed as C that
compiles (with period Watcom 9.5b) to byte-identical machine code. Files are named
**`FUN_<address>.c`** — the address is the ground-truth anchor back to the binary, Ghidra, and
`manifest/functions.json`; it never changes. What the directory *tree* adds is the organization the
original developers' project would have had: functions grouped by what they do, with the linked-in
**library** code separated from game logic.

Each directory has an **`_index.md`** listing its functions (address, size, match status, role).
For the narrative architecture, see [`docs/architecture.md`](../docs/architecture.md); for
field-level data structures, [`docs/object-model.md`](../docs/object-model.md).

## Game subsystems

| dir | what it is |
|-----|------------|
| `startup/`     | program & session startup: command-line/`main`, session init, timer & keyboard install |
| `sys/`         | memory, file I/O, decompression, DPMI/DOS services (engine infrastructure) |
| `input/`       | keyboard & mouse; mission cursor / target-action resolver |
| `entity/`      | entity/agent/vehicle pool + spatial-grid threading, spawns, chain walks |
| `map/`         | tile / isometric passability, column lookup, minimap & radar render |
| `combat/`      | weapons, damage, projectiles, line-of-sight, targeting (`0x34xxx` cluster) |
| `mission/`     | command / mission / orders interpreters (the scripting layer, jump-table dispatchers) |
| `economy/`     | economy, equipment, research, funding, target claiming, save-game |
| `render/`      | low-level drawing primitives: blit, VGA, masked copy, gauge, lines |
| `ui/`          | menus, panels, HUD, text engine (glyph / word-wrap), widgets |
| `multiplayer/` | NetBIOS-over-DPMI session setup, NCB send/receive, sync barriers |
| `anim/`        | animation tick / frame counters, palette-flash effects |
| `sound/`       | game-side sound system: driver load/init, XMIDI init, channel select |

## Library / non-game code (`lib/`)

Code that is **not game logic** — linked-in runtime and low-level primitives. Kept separate so the
game code reads clean. These are matched by db-transcription and made relink-safe by `tools/unbake.py`.

| dir | what it is |
|-----|------------|
| `lib/runtime/` | the linked-in Watcom C runtime (CLIB3S): string/mem/stdio/math, DOS-asm primitives |
| `lib/gfx/`     | low-level graphics/math primitives (different toolchain, `0x40000+`) |
| `lib/sound/`   | low-level sound-driver dispatch stubs (hand-asm, AIL-style) |

## `unclassified/`

~100 functions understood only by their call-graph position, not yet confidently placed in a
subsystem. As decoding continues they move out of here. **Grouping is best-effort** — the address in
the filename is the authoritative identity, the directory is a navigational aid.

## Tooling note

The build/match tools locate a function's source by name across all subdirectories
(`src/**/FUN_<addr>.c`), so moving a file between subsystems is safe — update the manifest `src`
path (matched functions) and the tools follow.
