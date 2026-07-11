# The Watcom compiler and toolchain

## What Watcom is

**Watcom C/C++** is the compiler the game was built with. It was a commercial
compiler from the late 80s and early 90s, and it was *the* compiler for serious DOS
games. Doom, Quake, and Descent were all built with it. It was open-sourced in 2003
and lives on as **Open Watcom**.

A compiler is the program that turns C source code into machine code. Watcom turns
our C into the same kind of bytes the original game is made of, which is exactly
what we need.

## Why the exact compiler matters

This is the heart of a matching decompilation. Different compilers, and even
different versions of the same compiler, turn the same C into different machine
code. They make different choices about instructions, registers, and layout.

So we can't reach for any modern compiler. We have to use the same Watcom the game
was built with, which we've since pinned down to **version 9.5** from the early 90s.
For a long time we used 10.0a, which matched everything until we found a function
whose flag-bit test only 9.5 compiles the original's way. It turns out 9.5 and 10.0a
produce identical code for most functions, so the difference only shows on certain
shapes. We also tried the modern Open Watcom and found its register choices have
drifted over the decades, so its output no longer matches. We use a preserved period
copy of 9.5 instead. There's more on this in [compiler flags](compiler-flags.md),
[register allocation](register-allocation.md), and the [journal](journal.md).

## The tools

"Watcom" is really a set of tools that work together. The ones we use:

- **wcc386** — the C compiler itself. Turns a `.c` file into an object file.
- **wdis** — the disassembler. Shows the machine code inside an object file as
  readable instructions, which is how we see what our C actually produced.
- **wlink** — the linker, which combines object files into a finished program.
- **wlib** — the librarian, for pulling apart the runtime library. Handy for the
  [game vs library](game-vs-library.md) work.
- **DOS/4GW** — the extender it bundles, covered on the
  [DOS page](dos-and-dos4gw.md).

## How we run a 30-year-old compiler

Watcom's compiler is itself a DOS program, so we can't just run it on a modern
machine directly. We run it inside **DOSBox**, a DOS emulator, which is itself inside
a container so the whole setup is reproducible. It's a bit of a Russian doll, a
period compiler inside a DOS emulator inside a container, but it means anyone can
rebuild the exact toolchain and get the exact same bytes back.
