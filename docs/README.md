# Syndicate decompilation: my learning notes

This is a personal project to learn how decompilation actually works. I'm taking
the original 1995 DOS game *Syndicate* and rebuilding it as C source that compiles
to the exact same machine code, one function at a time.

I'm new to all of this and I'm not a reverse engineer. These pages are my own
notes, written so I can understand each idea from scratch and come back to them
later when I've forgotten.

## What the project actually is

The game is a compiled program from 1995, a big block of machine code. I don't have
the source it was built from. The goal is to write C that, compiled with the same
old compiler, produces the *identical* bytes. Not code that behaves the same, code
that *is* the same.

The point of being that strict is certainty. If my C compiles to the original
bytes, it does exactly what the original does in every case, because it is the
original. There's nothing to test and no edge cases to worry about. When a function
matches, it's genuinely finished. The page on
[matching decompilation](matching-decompilation.md) goes into why that's worth the
extra effort.

Once enough of it is rebuilt this way, the whole game builds from source, and after
that it could be ported to a modern machine with real understanding of every line.

## Start here

If the ideas are new, read them in roughly this order. Each page defines its terms
as it goes.

- **[The CPU, registers, and instructions](cpu-basics.md)** — the foundation the
  rest sits on. Come back here whenever a register or instruction is unfamiliar.
- **[Matching decompilation](matching-decompilation.md)** — the goal, and why
  byte-for-byte rather than just "works the same".
- **[Stack frames](stack-frames.md)** — framed vs frameless functions, and what a
  stack frame even is.
- **[Calling conventions](calling-conventions.md)** — how a function receives its
  arguments, on the stack or in registers.
- **[Relocations and OMF](relocations-and-omf.md)** — why addresses are blanks until
  link time, and how we compare around them.
- **[Compiler flags](compiler-flags.md)** — what the flags mean, and how we worked
  out which ones the game was built with.
- **[Register allocation](register-allocation.md)** — why the compiler's choice of
  which register holds which value is the hardest thing to match.
- **[Game code vs the library](game-vs-library.md)** — a program is the game plus
  the compiler's runtime, and we only decompile the game.

## Quick glossary

- **Machine code** — the raw bytes the CPU runs. The game is a big block of it, and
  our job is to produce the same bytes from C.
- **Disassembly** — machine code shown as readable instructions like
  `mov eax, [esp+4]`. A view of the bytes, not the bytes themselves.
- **Decompilation** — going further, from machine code back towards C source.
- **Matching** — our discipline, where the C must compile to the *exact* original
  bytes, not just behave the same.
- **Relocation** — a spot where an address gets filled in later (at link or load
  time) rather than when the function is compiled.
- **Watcom** — the C compiler the game was built with. We run a preserved copy to
  reproduce its output.
- **OMF** — Object Module Format, the object-file format Watcom produces. Reading it
  tells us exactly where the relocations are.
