# Syndicate — native port

The modern, native build of the decompiled *Syndicate* engine. It reuses the
byte-faithful **game logic** from the decompilation (`../src`, the reference on `main`)
and replaces the DOS platform layer with a portable shim over SDL2 + a GPU backend, so
the game runs natively on Windows/macOS/Linux with hardware-accelerated presentation.

This branch **abandons byte-matching on purpose** (see `../BRANCHES.md`). It is a product.

## Feasibility (measured)

Of the 233 game-logic C files (excluding the `src/lib` assembly layer):

- **205 compile with modern gcc/clang unchanged** — no Watcom pragmas, far pointers,
  DOS interrupts, or port I/O. The game logic is portable as-is.
- **~28** touch the platform boundary (`int386` DOS calls, `__far` pointers, `inp`/`outp`,
  a few inline-asm `#pragma aux`). Those are exactly what the platform shim replaces.

So this is a **port, not a rewrite**: keep the logic, swap the hardware layer.

## Architecture

```
  game logic (../src/**, portable C)  ->  calls the platform interface (port/platform.h)
                                           |
                    DOS build (main):  the src/lib assembly layer (VGA/PIT/DOS int21h)
                    native build (here): port/platform_sdl.c (SDL2 video/input/audio/timer)
```

The game reads its data with the same code either way; only the boundary changes.

## Layers to replace (the shim)

| DOS layer | native replacement |
|---|---|
| VGA mode-X blitters -> video memory | software framebuffer -> GPU texture (SDL2/OpenGL) |
| INT9/INT33 keyboard/mouse | SDL events |
| INT8 PIT timer | SDL timer / high-res clock |
| AdLib/SoundBlaster/MIDI driver | SDL audio / SDL_mixer |
| `int 21h` file I/O, DOS/DPMI memory | stdio, native malloc |

## Build (planned)

CMake compiles the portable game logic from `../src` + `port/platform_sdl.c` into a native
executable. Requires the user's own *Syndicate* data files at runtime (see `../BRANCHES.md`).

## Status

**Native binary builds and runs.** `tools/port_bringup.py` compiles all 205 game-logic
objects, generates placeholder globals + weak boundary stubs, and links `port/build/syndicate`
-- a native executable that starts and exits. The compile->link->native-exe pipeline is proven.

Link surface measured: **351 globals + 90 boundary functions**. Remaining to make it PLAY:
1. **DGROUP data model** -- replace the BSS-placeholder globals with the real initialised data
   image at correct offsets, so overlapping field-views (g_syndicate_owner == g_syndicate_recs+2)
   alias like the DOS build. The meaty correctness step.
2. **Platform shim** -- replace the 90 stubs with real backends: video (SDL2 + GPU-textured
   framebuffer) -> input (SDL) -> timing (SDL) -> audio (SDL); and port the CLIB utilities
   (isqrt32, copy_bytes, filelength) as plain C.
3. Wire `main()` to the game's real main loop and load the data/ files.

Then it plays natively.
