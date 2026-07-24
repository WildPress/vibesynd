# Running the game natively

The matching decompilation proves the code is right by rebuilding the original DOS binary
byte for byte. This page is about a different goal on the `port` branch: taking that recovered
code and running it as a real native program, with no DOS and no DOSBox underneath it.

It works, on Linux and on Windows. The game boots through its own startup, loads its data from
the user's own copy, plays the intro, and draws the Syndicate title screen, all from its own
machine code. On Windows it runs as a single native 32-bit `.exe` with a live SDL window, no
DOSBox, no WSL, and no separate helper process.

![The Syndicate title screen, rendered by the game's own code in a native process](img/native-title.png)

The rest of this page explains why that was hard, the shape of the solution, how the same code
runs on both platforms, and how the pieces fit together.

## Why it is hard

The obvious plan for a port is "keep the C, replace the platform layer". That plan has a hole.
The parts of the game that actually matter at runtime, the main loop, the rendering, the menus,
the entity tick, and every one of the low-level blitters, are not C. They are hand-written
assembly, carried in the build as byte-for-byte transcriptions (see
[game vs library](game-vs-library.md)).

Those transcriptions have addresses baked into them. A `call` inside them points at a fixed
offset, and a read from a global points at a fixed data address, both fixed for the original
memory layout where the code loads at 0x10000 and the data sits at its DOS/4GW addresses. The
DOS build works because [`origbuild.py`](matching-playbook) forces exactly that layout. A modern
operating system will load the program somewhere else entirely, so every baked address points at
nothing.

So a native port has two problems to solve at once. It has to make the game's own assembly run
at a native address, and it has to give that assembly a platform to talk to that is not DOS
hardware.

## The shape of the solution

The port runs the game's real machine code and replaces only the edges. Three layers do the work.

```mermaid
flowchart TD
    subgraph native["native 32-bit process"]
        BLOB["the game, as one relocatable code blob<br/>(object1, symbolic data refs)"]
        DATA["the DGROUP data image<br/>(globals + relocated data pointers)"]
        SHIM["platform shims (plain C)<br/>file, video, memory, timer, input"]
        EMU["DOS-int / port-IO / DAC emulator<br/>(fault trap: signals on POSIX, VEH on Windows)"]
    end
    BLOB -->|calls its own routines| BLOB
    BLOB -->|reads/writes globals| DATA
    BLOB -->|calls a shimmed leaf| SHIM
    BLOB -->|int 0x21 / in / out| EMU
    SHIM --> SDL["render + input backend<br/>(SDL2 / headless frame dump)"]
    EMU --> SDL
```

The blob is the game. The data image is its world. The shims and the emulator are the DOS
platform, rewritten. Nothing here is a reimplementation of the game logic. It is the original
code, relocated, given somewhere to run.

## Making the assembly relocatable

The enabling trick is to stop treating the transcriptions as fixed bytes and start treating them
as relocatable code the native linker can place anywhere.

`tools/asm_symbolize.py` (on the `port` branch) rewrites a function so that its two kinds of
baked reference become symbolic. Every absolute data reference, which we already know from the
game's own relocation table, becomes `__obj<N> + offset`. Every call to another function becomes
a symbol the linker resolves. Everything else stays as raw bytes, so the internal layout is
untouched and self-relative jumps inside the function stay correct.

The proof this is right is a round trip. Assemble the rewritten function, link it at the original
address, and the bytes come back identical to the original. Across the whole binary, 273 of 275
transcriptions reproduce byte for byte. The two that don't are jump-table dispatchers, which the
next idea handles anyway.

That next idea is what actually runs. Rather than link functions one at a time, the port emits
the entire code segment as one contiguous blob at its original relative offsets
(`tools/asm_emit_blob.py`). Keeping the layout means every jump and call inside the code stays
valid without any work, and every absolute code pointer, including the switch-jump tables that
defeated the per-function approach, resolves as a simple `__obj1 + offset`. Only the data
references need symbols. This is the model the DOS build uses, rebuilt as a native object.

## The data the code reads

The code addresses its globals as offsets into one big data segment, DGROUP.
[`tools/port_dgroup.py`](resources-and-data-files) builds that segment from the original data
pages and places the per-object bases at their true offsets, so overlapping views of the same
memory still line up.

Data holds pointers too. String tables and other data-to-data pointers are stored, like the code
references, as unrelocated placeholders. The same fixup pass that the DOS loader would run has to
run here. So the data image is emitted with each of those 851 pointers rewritten to `__obj<N> +
offset`, exactly like the code, and the native linker relocates them. Without this, the first
pointer the game follows at startup is null and it crashes on a message string.

## The platform, rewritten

With the code relocated and the data placed, the game runs. It then immediately tries to talk to
DOS hardware, which is where the shims come in.

A useful discovery made the shims simple. The game was compiled with Watcom's stack calling
convention, not the register one, so its routines take their arguments on the stack just like
ordinary cdecl C. That means a shim is a plain C function. There is no need for assembly glue
between the game and the platform.

The port redirects the low-level leaves to C. Each one has its entry in the blob overwritten with
a jump to a `shim_` function.

- **File I/O** maps onto POSIX file descriptors. The game's `open`, `read`, `write`, and `seek`
  become the obvious system calls.
- **Video** routes the frame to the render backend. The game composites into a linear offscreen
  buffer and calls a present routine (see [blitters](blitter.md)). The shim hands that buffer to
  SDL as a texture instead of pushing it through mode-X VGA.
- **Memory** hands out real memory. Crucially it uses executable mappings, because the game loads
  code overlays into allocated buffers and jumps into them.
- **Timer** runs a background thread that advances the tick the game waits on, the way the
  hardware timer interrupt used to.

The blob symbols are all prefixed so they cannot collide with the C library. Without that, the
game's own `open` shadows the real one and the file shim calls itself forever.

## The DOS calls that are left

Some of the game's own library helpers still issue a raw `int 0x21`, or read and write hardware
ports directly. In a native process those instructions fault. Rather than shim every one of those
functions, the port catches the fault and services the instruction in place.

The handler does the small thing the instruction meant, updates the registers, steps the
instruction pointer past it, and returns. On a DOS call, a BIOS call, a port read or write, or a
privileged instruction like `cli`, execution carries on as if the hardware had answered.

The servicing logic is one file (`port/dosint_core.c`) that works on a plain register array, so
it is identical on every operating system. Only the way a fault is caught differs, so each
platform gets a thin adapter around that core:

- On POSIX (`port/dosint.c`) a `SIGSEGV` / `SIGILL` handler reads the trapped registers from the
  signal context.
- On Windows (`port/dosint_win.c`) a Vectored Exception Handler does the same from the exception
  context. The game's instructions surface there as two exception codes: the `int` calls as
  access violations, and `in` / `out` / `cli` as privileged-instruction faults.

Two parts of this matter for the picture on screen. The port reads and writes are mostly ignored,
except the VGA palette registers, which the handler captures so colours are right. And the intro
copies its frames straight to the old VGA memory address, so the port maps a page there for the
writes to land in.

## The drivers the game loads

Syndicate ships two loadable drivers, `gamedg.dll` for graphics and `gamefm.dll` for sound. They
are not native libraries. They are DOS executables the game reads off disk, relocates, and runs.
Running them natively would mean building a small DOS program loader, and their code reaches for
real hardware anyway.

The port takes the simpler road. It fails the driver open, and the game degrades to its own
built-in render path, which is exactly the path the shims already cover. This is why the title
screen appears without the graphics driver ever loading.

## How the boot actually goes

Put together, a run walks this path, and each step was a wall cleared in turn.

```mermaid
flowchart LR
    A["call startup_main"] --> B["C library init<br/>(DOS ints emulated)"]
    B --> C["load startup data<br/>(RNC-decompressed)"]
    C --> D["allocate memory<br/>(executable)"]
    D --> E["skip the .dll drivers"]
    E --> F["load menu + title assets"]
    F --> G["play the intro FLIC<br/>(palette via the DAC)"]
    G --> H["draw the title screen"]
```

The build script `port/build_boot.sh` runs the whole thing and drops the rendered frame to a
file. It reads the game's data from a folder of the user's own files, so no assets are shipped,
the same rule the DOS build follows.

## The same code on Windows

Everything above describes one native process. The interesting claim is that it is the *same*
process on Windows, running the game's own assembly, not a reimplementation and not a
cross-compiled subset. Getting there needed the fault emulator's Windows adapter above, plus two
boundary fixes at the point where the C toolchain meets the game's assembly.

The first is a naming convention. A 32-bit Windows object file (COFF) prefixes every C symbol with
an underscore, so the C variable `__obj2` becomes the symbol `___obj2`, and a call to
`rnc_decompress` looks for `_rnc_decompress`. The emitters wrote bare names, so nothing linked.
The fix is a switch (`--underscore`) that makes them prefix every symbol for a Windows target, so
the assembly and the C resolve to each other again. On Linux the names stay bare.

The second is a register convention, and it is the subtler one. The game's assembly follows
Watcom's rules, where `eax`, `ebx`, `ecx` and `edx` are scratch: a called routine may clobber them
freely. The C world, on both Linux and Windows, treats `ebx` as callee-saved: a called routine
must leave it as it found it. So when the C compiler parked a live value in `ebx` across a call
into the game's assembly, the assembly trashed it. On Windows this first showed up as a wrong
result, then as a crash once a pointer happened to land in `ebx`. The fix is to compile any C that
calls into the assembly with `-ffixed-ebx`, which tells the compiler never to use `ebx`, so there
is never a live value there to lose. It found this bug on Windows, but the same rule protects the
Linux build too.

With those two in place, the whole game links as a native Windows binary and boots exactly as it
does on Linux.

```mermaid
flowchart TD
    SRC["the game's transcribed assembly<br/>+ the platform C"]
    SRC -->|"asm_symbolize / asm_emit_blob --underscore"| COFF["COFF objects<br/>(underscore-prefixed symbols)"]
    SRC -->|"-ffixed-ebx"| CC["C compiled with ebx reserved"]
    COFF --> LINK["i686-w64-mingw32 link"]
    CC --> LINK
    LINK --> EXE["one native .exe"]
    VEH["Vectored Exception Handler<br/>(dosint_win.c)"] --> EXE
    SDL2["in-process 32-bit SDL2"] --> EXE
```

## One window, one process

The Linux window was built as two processes: the 32-bit game publishing frames into shared memory,
and a separate 64-bit SDL viewer drawing them. That split existed for one reason, that the SDL on
the WSL desktop was 64-bit and the game is 32-bit, so they could not share an address space.

Windows has no such constraint. A 32-bit MinGW build of SDL2 links straight into the 32-bit game,
so the window lives in the game's own process (`port/sdl_display.c`). The game runs on one thread,
and the display side creates the window, pumps keyboard and mouse, and presents each frame on
another. SDL wants all of its calls on a single thread, so the backend sets itself up on the first
frame, on the thread that will own the window from then on.

The result is `syndicate_win.exe` plus `SDL2.dll`: double-click, and the game's own code draws the
title screen in a real window, with keyboard and mouse wired in.

## The render layer on its own

Before any of the boot worked, the render backend was built and proven on its own. It reads a
real Syndicate screen and its palette from the user's data, decompresses the palette with a C port
of the game's own RNC depacker, converts the 8-bit image through the palette, and shows it as a
GPU texture in an SDL2 window. Its output is byte-identical to a reference renderer. That first
proof is also what confirmed the render path would cross-compile to Windows at all.

## Building and running it

The port branch carries the scripts. All of the Windows builds run in the Docker image
(`docker/port.Dockerfile`), which carries the 32-bit toolchain, the MinGW cross-compiler, and a
32-bit SDL2, so the build is reproducible and needs nothing installed on the host. The resulting
`.exe` runs on Windows directly.

Native Linux, from a checkout of `port` with a 32-bit toolchain and the user's own data to hand:

- `bash port/build_boot.sh` boots the game and renders the title screen to `frame.ppm`.
- `bash port/build_asm_native.sh` runs the game's own RNC decompressor natively, as the smallest
  proof that the relocated assembly executes.

Native Windows, built in the container, run on Windows:

- `docker/win_window_build.sh` builds `syndicate_win.exe`, the live window. Run it from a folder
  that holds a lowercase `data` directory of the user's files.
- `docker/win_boot_build.sh` builds the headless boot, which renders one frame to a file. Useful
  for checking the boot without a window.
- `docker/win_blob_proof.sh` and `docker/win_rnc_proof.sh` are the two smallest proofs: that the
  whole code blob links as a Windows object, and that a single game routine runs as a Windows
  `.exe`.

## What is left

The title screen is up in a live window on both platforms, so the next work is depth, not a new
wall.

- **Into a mission.** The path from the title through the menus and the world map into a running
  mission is exercised on Linux but not yet driven all the way through on Windows.
- **Sound.** The audio shims are stubs. Real sound means either the `gamefm.dll` driver or a
  reimplementation of the mixer.
- **The driver overlays.** The port fails the driver open and uses the game's built-in render
  path. Running the real overlays would need a small DOS program loader.
- **Portable game logic.** The hot path is still the game's transcribed assembly. Turning that
  into portable C is what would let the port target macOS and other architectures, and it is the
  larger project the byte-exact work on `main` is the reference for.

The branch model is deliberate. The byte-exact decompilation stays on `main` as the reference.
The port lives on `port` and is free to stop matching bytes, because its job is to run, not to
prove. See [the branch notes](https://github.com/WildPress/vibesynd/blob/main/BRANCHES.md).
