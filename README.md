# Syndicate — Matching Decompilation

A **matching decompilation** of the original 1993/95 DOS *Syndicate*
(`SYNDICAT_MAIN.EXE`): hand-written C that, compiled with the period Watcom
toolchain, reproduces **byte-identical** machine code to the original binary —
one function at a time.

This is a clean-room effort. It works only from the compiled binary (which you
must own) and its disassembly — never from any original source code.

## Goal — two phases

1. **Matching decompilation (current).** Reconstruct the game as C source that
   assembles to the *exact* original bytes. The binary's own machine code is the
   source of truth; success is zero-diff parity, tracked in
   `manifest/functions.json`. When this is done, the game builds from source and
   plays identically to the 1995 release.
2. **Modern port (future).** Once it builds from source, port it to modern operating
   systems with quality-of-life improvements. The matching phase is what makes a
   faithful, well-understood port possible.

## Legal / hygiene

- The project works purely from **static and dynamic analysis of a binary you own** —
  no original source code is used or referenced.
- The copyrighted game binary and extracted segments (`inputs/`), the Ghidra
  databases (`ghidra/`), and the abandonware Watcom toolchain (`toolchain/`) are
  **git-ignored and never committed**. This repository contains only our own tooling,
  our written C, and analysis metadata. **To use it you must supply your own copy of
  the game.**
- *Syndicate* and Watcom are © their respective rights holders; this project is
  independent and not affiliated with or endorsed by them.
- Reverse-engineering an owned binary for interoperability / a compatible
  reimplementation is a long-established practice, but its legal status varies by
  jurisdiction. This is a hobbyist research project, not legal advice.

## The target

- **Compiler:** period **Watcom C/C++ 10.0-family**. Game code is overwhelmingly
  *frameless*; the C runtime library (CLIB) is linked in near the top of the code
  segment (see *Game vs. library* below).
- **Format:** Linear Executable (`.LE`), 32-bit protected mode, flat model, DOS/4GW
  extender. `SYNDICAT_MAIN.EXE`, ~518 KB, dated Feb 1995.
- **Segments** (extracted to `inputs/`, git-ignored): OBJECT1 (code, ~261 KB),
  OBJECT2 (data, ~53 KB), OBJECT4 (data/BSS, ~114 KB).

## How matching works

The oracle is **byte equality** of a function's machine code. We write C for a
function, compile it with the period Watcom compiler (headless under DOSBox in the
`synd-decomp` container), disassemble the result, and diff it (relocation-aware)
against the original bytes. `RELOC-AWARE match: YES` means a match; it's recorded in
the manifest.

- **Frameless functions** (the bulk of the game): compile with `-4s`/`-4r`
  `-oneatx -zp8 -s -zq`, no `-of`.
- **Framed forwarders / call-sequences**: `-3s -of` (frame pointer, 386 codegen).
- See `AGENTS.md` for the full working notes, toolchain provenance, and per-class
  recipes.

## Game vs. library boundary

The top of the code segment (`0x3a000`–`0x3e600`) is the **linked-in Watcom C
runtime library** — `strcpy`, `tolower`, `fopen`, buffered/DOS I/O, etc. — *not*
game code. These were identified by byte-matching against the original `CLIB*.LIB`
(`tools/libmatch.py` / `tools/libname.py`), are listed in
`manifest/library_functions.md`, and are tagged **`LIBRARY`** in Ghidra.

- For **matching**, they're reproduced by linking the original library, not by
  hand-writing C — so the decompilation effort targets the game's *own* code.
- For the **future port**, they map straight onto the modern platform's libc (the
  handful of DOS-specific ones become ordinary OS calls).

## Directory layout

```
inputs/          original binary + extracted segments      (git-ignored)
ghidra/          Ghidra project / databases                (git-ignored)
toolchain/       period Watcom compilers (abandonware)     (git-ignored)
docker/          Dockerfile for the pipeline image
ghidra_scripts/  headless inventory + extraction scripts
tools/           matching harness + analysis scripts
src/             our written, matched C source
manifest/        functions.json (inventory + status),
                 library_functions.{md,json} (runtime-lib map)
dashboard/       local coverage cockpit (not published)
build/           compiled objects + diffs                  (git-ignored)
run.sh           host wrapper: docker run into the image
```
