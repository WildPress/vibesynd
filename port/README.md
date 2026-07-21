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

## Making the asm run natively (the relocation pipeline)

The catch above — 275 hot-path functions are asm with offsets baked for the DOS layout — has a
mechanical fix. `tools/asm_symbolize.py` rewrites a transcription into **relocatable** assembly:

- every absolute data ref (a type-7 LE fixup) becomes `.long __dgroup + <flat addr>`, and
- every inter-function call/jump (found by disassembly) becomes `.long <callee> - . - 4`,

while all intra-function bytes are emitted verbatim, so the layout is preserved and self-relative
offsets stay valid. The native linker then resolves the symbols — to the C reimplementation of a
routine where one exists, or to the symbolized asm otherwise.

Verified by a byte-identity round trip (assemble → link at the original address → compare to the
original bytes): **273 of 275 transcriptions reproduce byte-for-byte** (`tools/asm_symbolize_batch.py`;
the 2 failures are jump-dispatch functions with a trailing table). And it doesn't just match — it
**runs**: `port/build_asm_native.sh` symbolizes the RNC decompressor, links it with C, and the game's
own `rnc_decompress` machine code decompresses a real palette correctly in a native 32-bit process:

```
bash port/build_asm_native.sh    # -> "NATIVE-ASM-OK: the game's assembly decompressed a 768-byte palette"
```

Caveat: the ~64 transcriptions that touch hardware (VGA ports, `int 21h`, the PIT/keyboard/mouse
ISRs) still need a C shim behind `platform.h` — symbolizing them makes them link, not work. But the
~211 pure-computation ones are now natively linkable, which is most of the hot path.

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
- [x] Asm-relocation pipeline: transcriptions -> relocatable, byte-identical (asm_symbolize.py)
- [x] **The game's own asm runs natively** (symbolized rnc_decompress decompresses a palette, -m32)
- [x] A genuine Windows `.exe` (MinGW + Windows SDL2), `port/build_win.sh` -> syn-demo.exe + SDL2.dll
- [x] **Whole game code links natively**: all 568 fns + 42 gap blobs + 53 internal labels ->
      one object, 0 undefined code symbols (`port/build_asm_all.sh`; 663 text symbols, all byte-identical)
- [x] Switch jump tables / code pointers solved: emit object1 as ONE contiguous blob at its
      original offsets (`tools/asm_emit_blob.py`), so every `__obj1 + off` code pointer resolves.
      Byte-identical (261620 bytes, 9204 fixups symbolized).
- [x] **Whole game (code + DGROUP data model) links into a native 32-bit executable, 0 undefined
      symbols** (`port/build_native.sh`: 261KB text + 1.08MB data). Not yet runnable.
- [x] Shim-redirect mechanism: `asm_emit_blob.py --shims` replaces a DOS leaf's blob entry with
      `jmp shim_<name>`; blob symbols prefixed `game_` so they don't collide with libc
- [x] **The game's own file loader runs natively** (`port/build_loader.sh`): game_load_unpack_file
      opens + reads + RNC-decompresses a real file off disk via POSIX shims (port/shims_file.c).
      Confirmed: the game is -4s (STACK calling), so shims + entry are plain cdecl C.
- [x] Video shims -> SDL layer: vga_planar_present / present_* read g_screen_buf (DGROUP 0x5368)
      and route to the GPU framebuffer; upload_palette captures the DAC palette; set_video_mode /
      clear / cursor-bg / vsync handled. Memory shims (os_getmem/brk/heap_grow) -> malloc.
- [x] **The game's own present path drives a real frame natively** (`port/build_video.sh`):
      game_vga_planar_present presents g_screen_buf, palette via the game's own RNC loader --
      byte-identical to the reference render. (Headless PPM; a 32-bit SDL window is a follow-up.)
- [ ] Timer (PIT) + input (poll_key/mouse) shims
- [ ] Wire the entry to the game's startup (skip DOS video-mode/DPMI init) + load `data/`
- [ ] Reach the game's render into g_screen_buf -> present via SDL  ->  **port-v0.1.0** (live frame)
- [ ] Input, timing, audio backends  ->  **port-v1.0.0** (plays a mission natively)
