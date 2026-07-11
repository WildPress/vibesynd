# My notes on how this works

These are my own notes for this project. I'm doing it to learn how decompilation
actually works, and I'm new to all of it. I'm not a reverse engineer, so this is
written for me to understand the ideas from scratch and to come back to later when
I've forgotten them.

Each note explains one concept in plain language and defines the terms as it goes.
They're meant to be read in order the first time, but each one stands on its own.

(The terse working reference, the kind of thing an experienced person would want,
is `AGENTS.md` in the repo root. This folder is the version for me.)

## The concepts, in a sensible reading order

1. [What a matching decompilation is](matching-decompilation.md) — the goal, and
   why we insist on byte-for-byte identical output.
2. [Stack frames: framed vs frameless](stack-frames.md) — what a stack frame is,
   and why some functions have one and some don't.
3. [Calling conventions](calling-conventions.md) — how a function receives its
   arguments, on the stack or in registers.
4. [Relocations and the OMF differ](relocations-and-omf.md) — why addresses are
   blanks until link time, and how we compare around them.
5. [Compiler flags](compiler-flags.md) — what the flags mean, and how we worked
   out which ones the game was built with.
6. [Register allocation](register-allocation.md) — why the compiler's choice of
   which register holds which value is the hardest thing to match.
7. [Game code vs the library](game-vs-library.md) — a program is the game plus the
   compiler's runtime, stapled together, and we only decompile the game.

## A quick glossary

- **Machine code**: the raw bytes the CPU executes. The original game is a big
  block of it. Our job is to produce the same bytes from C.
- **Disassembly**: machine code translated back into human-readable instructions
  like `mov eax, [esp+4]`. It's a view of the bytes, not the bytes themselves.
- **Decompilation**: going one step further, from machine code back to something
  like C source.
- **Matching**: our specific discipline, where the C has to compile to the *exact*
  original bytes, not just behave the same way.
- **Relocation**: a spot in the code where an address gets filled in later, by the
  linker or the loader, rather than being fixed when the function is compiled.
- **Watcom**: the C compiler the game was built with, back in the early 1990s. We
  run a preserved copy of it to reproduce its output.
- **OMF**: the object file format Watcom produces. Knowing how to read it lets us
  find exactly where the relocations are.
