# Syndicate decompilation — v1.0.0

A matching decompilation of Bullfrog's *Syndicate* (1993, DOS, `MAIN.EXE`, 518,713 bytes,
Watcom C/C++ 9.5). The reconstructed engine builds into a runnable DOS/4GW program that plays
the original game.

## What works

Built via `tools/origbuild.py` into `GAMEO.EXE` and run under DOS/4GW, the engine:

- Boots through the DOS/4GW extender (genuine Watcom `__x386_start`).
- Loads the original game data files at runtime.
- Renders the title screen, main menu, and world-map mission-selection screen.
- Responds to keyboard and mouse; the mission timer (INT8) runs.

Verified end-to-end against a retail data set: title -> main menu -> world map, interactive,
no faults.

## Decompilation status

- 450 / 553 tracked functions byte-exact; 102 behaviourally-equivalent (register-role /
  scheduling / encoding codegen ties, each with a dated equivalence verdict); 1 latent
  16-bit-arith quirk documented. ~63% byte-exact, ~77% byte-correct.
- Compiler settled: Watcom 9.5, `-4s -oneatx -zp8 -s -zq` (game code); the C runtime is the
  386-built Watcom CLIB; see `docs/compiler-version.md` and `docs/compiler-flags.md`.

## You need your own copy of the game

This release is the **engine only**. It contains no game assets. To play, supply the original
*Syndicate* data files — buy it from GOG (*Syndicate Plus*) — and place the `data/` directory
next to `GAMEO.EXE`. Nothing copyrighted is distributed here.

## Build & run

    python3 tools/origbuild.py          # -> build/GAMEO.EXE (in the synd-decomp container)
    # place DOS4GW.EXE + your data/ dir alongside GAMEO.EXE, run under DOSBox/DOS

## Known limits

- Deep in-mission gameplay (the isometric tactical map) is not yet exhaustively verified; a
  handful of db-transcribed callees may have rough edges.
- Not yet ported off DOS/4GW; a modern-OS port and modding support are the post-1.0 roadmap.
