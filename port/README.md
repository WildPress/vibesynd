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

Scaffolding. Feasibility proven (205/205 game-logic files compile). Next:
1. Generate C definitions for the globals (from `manifest/globals.json`) so the logic links.
2. Link a native binary against a **stub** platform (starts + exits).
3. Bring the platform up one subsystem at a time: video -> input -> timing -> sound, each
   checked against the DOS build's behaviour.
