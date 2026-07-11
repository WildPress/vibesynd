# Syndicate matching decompilation

A matching decompilation of the original 1993/95 DOS *Syndicate*
(`SYNDICAT_MAIN.EXE`). The aim is C source that, compiled with the period Watcom
toolchain, produces machine code byte for byte identical to the original, one
function at a time.

This is a clean-room effort. It works only from the compiled binary you own and
its disassembly, never from any original source.

New to this? The [`docs/`](docs/README.md) folder explains the concepts in plain
language: matching decompilation, stack frames, calling conventions, relocations
and OMF, the compiler flags, and register allocation.

## Progress

As of **v0.1.0**, game code is **0.73%** matched by bytes (753 of 102,740), across
**41 of 463** functions. Another 37 functions are the runtime library, which we'll
link rather than decompile. Byte coverage is the honest number, since early matches
are small and function count runs ahead of it.

Run `python3 tools/score.py` for the current figure, and see `CONVENTIONS.md` for how
we commit and version.

## Goal, in two phases

1. **Matching decompilation (current).** Rebuild the game as C that assembles to
   the exact original bytes. The binary's own machine code is the source of truth,
   and success is zero-diff parity tracked in `manifest/functions.json`. Once this
   is done the game builds from source and plays like the 1995 release.
2. **Modern port (later).** With the game building from source, port it to modern
   operating systems and add quality-of-life improvements. The matching phase is
   what makes a faithful, well-understood port possible.

## Legal and hygiene

The project works only from static and dynamic analysis of a binary you own. No
original source is used or referenced.

The copyrighted game binary and its extracted segments (`inputs/`), the Ghidra
databases (`ghidra/`), and the abandonware Watcom toolchain (`toolchain/`) are
git-ignored and never committed. This repository holds only our own tooling, our
written C, and analysis notes. To use it you need to supply your own copy of the
game.

*Syndicate* and Watcom belong to their respective rights holders. This project is
independent and has no affiliation with them.

Reverse-engineering a binary you own, to build a compatible reimplementation, is a
long-established practice, but the legal position varies by country. This is a
hobby research project, not legal advice.

## The target

- **Compiler:** period Watcom C/C++ 10.0-family. Most game code is frameless. The
  C runtime library is linked in near the top of the code segment, covered under
  *Game code versus library* below.
- **Format:** Linear Executable (`.LE`), 32-bit protected mode, flat model, DOS/4GW
  extender. `SYNDICAT_MAIN.EXE` is about 518 KB, dated February 1995.
- **Segments** (extracted to `inputs/`, git-ignored): OBJECT1 (code, about 261 KB),
  OBJECT2 (data, about 53 KB), OBJECT4 (data and BSS, about 114 KB).

## How matching works

The test is byte equality of a function's machine code. We write C for a function,
compile it with the period Watcom compiler (headless under DOSBox in the
`synd-decomp` container), disassemble the result, and diff it against the original
bytes. The diff is relocation-aware, so call and data addresses don't block a
match. A `RELOC-AWARE match: YES` means we have it, and it goes into the manifest.

Two recipes cover most of what compiles cleanly:

- Frameless functions, the bulk of the game, use `-4s` or `-4r` with
  `-oneatx -zp8 -s -zq` and no `-of`.
- Framed forwarders and call sequences use `-3s -of`.

The full working notes, toolchain provenance, and per-class recipes live in
`AGENTS.md`.

## Game code versus library

The top of the code segment, from about `0x3a000` to `0x3e600`, is the linked-in
Watcom C runtime library. It holds `strcpy`, `tolower`, `fopen`, buffered and DOS
I/O, and similar routines. It is not game code.

We identified these by byte-matching against the original `CLIB*.LIB`
(`tools/libmatch.py` and `tools/libname.py`). They're listed in
`manifest/library_functions.md` and tagged `LIBRARY` in Ghidra.

For matching we reproduce them by linking the original library rather than writing
C, so the decompilation effort can stay on the game's own code. For the future
port they map onto the modern platform's libc, and the few DOS-specific ones become
ordinary OS calls.

## Directory layout

```
docs/            plain-language explainers of the concepts
inputs/          original binary and extracted segments     (git-ignored)
ghidra/          Ghidra project and databases               (git-ignored)
toolchain/       period Watcom compilers (abandonware)      (git-ignored)
docker/          Dockerfile for the pipeline image
ghidra_scripts/  headless inventory and extraction scripts
tools/           matching harness and analysis scripts
src/             our written, matched C source
manifest/        functions.json (inventory and status),
                 library_functions.{md,json} (runtime-lib map)
dashboard/       local coverage cockpit (not published)
build/           compiled objects and diffs                 (git-ignored)
run.sh           host wrapper that runs docker into the image
```
