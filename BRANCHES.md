# Branch model

This repository holds two related but deliberately-different lines of work.

## `main` — the matching decompilation (like-for-like)

Every function reconstructs the original DOS *Syndicate* `MAIN.EXE` (Watcom C/C++ 9.5)
**byte-for-byte**, or behaviourally-equivalent where a codegen tie is unreachable. It
builds into a DOS/4GW executable via `tools/origbuild.py` that runs under DOSBox and
plays the original game from the user's own data files.

This branch is the **reference and verification target**. Its whole value is fidelity:
the byte-match proves the reconstruction is correct. Nothing that breaks the match lands
here. Readability work that is byte-neutral (naming globals/functions, comments) is fine;
behaviour changes and new features are not.

Tagged releases (`v1.0.0`, ...) come from this branch.

## `port` — modern OS, GPU, and quality-of-life

A native port derived from the decompilation. It **keeps the game-logic C** and
**replaces the DOS platform layer** (VGA blitters, keyboard/mouse interrupt handlers, the
sound/timer driver, `int 21h` file I/O, DOS/4GW memory model) with a portable shim over
SDL2 + a modern GPU backend. It compiles with a modern compiler (Clang/MSVC) into a native
Windows/macOS/Linux executable, with hardware-accelerated presentation (the game's
framebuffer uploaded as a GPU texture: resizable window, scaling, vsync).

This branch **abandons byte-matching on purpose** — it is a product, not a reference.
Quality-of-life features (higher resolutions, save anywhere, rebindable input, speed
controls) live here.

## How work flows

- Decompilation progress and byte-neutral readability land on `main`.
- The `port` branch periodically merges or cherry-picks game-logic improvements from `main`.
- Platform, rendering, and feature work happens only on `port`.

Both branches require the user to supply their own original *Syndicate* data files at
runtime (bought from GOG). No game assets are distributed by either branch.
