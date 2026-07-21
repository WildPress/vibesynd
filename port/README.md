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

### The catch: the critical path is still assembly

Compiling 205 *files* is not the same as running the *game*. 275 of the 568 functions —
including the whole hot path: the main loop (`main_game_loop`), the render
(`render_sorted_sprites`), the menu / world-map / briefing screens, the entity tick, and
**every** video primitive (`plot_point`, `blit_sprite`, `upload_palette`, …) — are still
byte-transcriptions: raw `db` machine-code with `call rel32` and `mov abs32` offsets baked
in for the original 0x10000-based layout. The DOS build runs them because `origbuild.py`
forces that exact layout; a native link places code and data elsewhere, so those offsets
point nowhere.

The port therefore grows in two independent tracks:
1. **The platform + render layer, reimplemented in portable C** (this is what's landing now).
2. **The game logic**, which only becomes callable natively as each hot-path routine is
   decompiled from asm to real C on `main`. Until then the native binary can render and
   drive the modern layer, but can't yet run the original game loop.

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

## First native render (done)

The modern render path now works end-to-end, natively, from the user's own data:

```
port/rnc.c          RNC ProPack method-1 depacker (C port of the game's own rnc_decompress,
                    validated: every palette -> 768 bytes, all <= 0x3f 6-bit VGA)
port/gfx_soft.c     palette 8-bit -> RGBA conversion + the blitters (plot/fill/blit) in C
port/platform_sdl.c SDL2 window + framebuffer uploaded as a GPU streaming texture (platform.h)
port/demo_render.c  loads MCONSCR.DAT (raw 320x200) + MSELECT.PAL (RNC) and shows it
```

Build and run (Linux/WSLg; a real Windows `.exe` needs a Windows SDL2 toolchain, a follow-up):

```
bash port/build_demo.sh                       # fetches SDL2 headers if missing, builds port/build/syn-demo
./port/build/syn-demo "<your DATA dir>"       # a GPU-backed window showing the briefing console
./port/build/syn-demo "<DATA>" --shot out.ppm # headless: dump one frame (verified byte-identical to ref)
```

The C render path is byte-for-byte identical to the reference renderer. This is the reusable
video shim; the blitters here (`gfx_soft.c`) are the portable-C stand-ins for the hand-asm
`plot_point`/`fill_bytes`/`blit_block` that the game logic will call once it's C on the hot path.

## Build (planned, full game)

CMake compiles the portable game logic from `../src` + `port/platform_sdl.c` into a native
executable. Requires the user's own *Syndicate* data files at runtime (see `../BRANCHES.md`).

## Status

**Native binary builds and runs.** `tools/port_bringup.py` compiles all 205 game-logic
objects, generates placeholder globals + weak boundary stubs, and links `port/build/syndicate`
-- a native executable that starts and exits. The compile->link->native-exe pipeline is proven.

Link surface measured: **351 globals + 90 boundary functions**. Remaining to make it PLAY:
1. **DGROUP data model -- DONE (layout + aliasing).** `tools/port_data.py` builds the real
   initialised DGROUP image (OBJECT2|zero|OBJECT4|zero, 1.06 MB) and `port/gen/globals.s`, which
   places all 479 referenced globals at their true offsets via `.set g_x, __dgroup + <addr>`.
   Overlapping field-views now alias (verified: g_syndicate_owner @0x539e == g_syndicate_recs @0x539c
   + 2). Data-internal pointers (tbl_* string tables, data->code function tables) still need a
   runtime fixup pass -- the remaining data step.
2. **Platform shim** -- replace the 90 stubs with real backends: video (SDL2 + GPU-textured
   framebuffer) -> input (SDL) -> timing (SDL) -> audio (SDL); and port the CLIB utilities
   (isqrt32, copy_bytes, filelength) as plain C.
3. Wire `main()` to the game's real main loop and load the data/ files.

Then it plays natively.

## Bring-up progress

- [x] Game logic compiles natively (205/205, modern gcc)
- [x] Links to a native binary (stub platform)
- [x] DGROUP data model: globals at true offsets, overlapping views alias (verified in binary)
- [x] 32-bit native binary with the real data model (correct pointer sizes)
- [x] RNC-1 depacker in C (loads the user's compressed palettes/screens) — validated
- [x] SDL2 video backend: 8-bit framebuffer -> palette -> GPU streaming texture, resizable window
- [x] VGA blitters (plot/fill/blit) + palette conversion reimplemented in portable C
- [x] **Renders a real Syndicate screen natively from the user's data** (byte-identical to reference)
- [ ] A genuine Windows `.exe` (MinGW/MSVC + Windows SDL2) — so far a Linux/WSLg build
- [ ] Runtime fixup pass for data-internal pointers (tbl_* tables, function tables)
- [ ] Decompile the hot-path asm (main loop, render, screens) to C on `main` so it links natively
- [ ] Wire `main()` to the game's real main loop + load `data/`  ->  **port-v0.1.0** (renders a live frame)
- [ ] Input, timing, audio backends  ->  **port-v1.0.0** (plays a mission natively)
