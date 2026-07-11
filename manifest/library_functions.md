# Library functions in the `0x3a000+` region — identified

The framed functions clustered at the top of OBJECT1 (`0x3a000`–`0x3e600`) are a **mix**
of linked-in **Watcom C runtime library** code and the game's own thin wrapper functions.
This file records which is which, and names the library ones.

**Method** (`tools/libname.py`, machine-generated → `manifest/library_functions.json`):
for each framed function (`55 89 e5` or `53 55 89 e5` prologue), slide a 12-byte window
over its bytes and search the unpacked Watcom `CLIB3R/S.LIB` (9.5b + 10.0a). *Coverage* =
fraction of windows found verbatim in a library. The module name is the CLIB module holding
the **longest contiguous run** of the function's bytes (from `wlib` module offsets).

**Confidence:**
- **≥90% coverage** — near-certain: the function's exact code is a CLIB module. Name reliable.
- **40–89%** — definitely library (relocations lower coverage); module name is best-effort
  (one Watcom source module holds several functions, so the name is the *source module*,
  not necessarily this exact symbol).
- **15–40%** (`library?`) — probably library, low confidence; verify before trusting the name.
- **<15%** — the game's own code (a wrapper that *calls* the library). These match our compiler.

Regenerate: `docker run --rm -e WRITE_JSON=1 -v "$PWD":/work -w /work synd-decomp python3 tools/libname.py`

## Confirmed library — high confidence (100% coverage)

| addr | FUN | size | CLIB module |
|---|---|---|---|
| 0003a8d7 | FUN_0003a8d7 | 41 | **strcpy** |
| 0003aea6 | FUN_0003aea6 | 50 | **strncmp** |
| 0003aed8 | FUN_0003aed8 | 14 | **labs** (abs/labs) |
| 0003aef9 | FUN_0003aef9 | 63 | **stricmp** |
| 0003b22d | FUN_0003b22d | 12 | **outp** (port write) |
| 0003b3b9 | FUN_0003b3b9 | 45 | **segread** (segment regs) |
| 0003b972 | FUN_0003b972 | 44 | **fopen** (helper) |
| 0003b9ee | FUN_0003b9ee | 19 | **fclose** (helper) |
| 0003c44d | FUN_0003c44d | 32 | **isatty** |
| 0003cbf9 | FUN_0003cbf9 | 45 | **spawnve** (helper) |
| 0003da37 | FUN_0003da37 | 21 | **tolower** |
| 0003dce5 | FUN_0003dce5 | 21 | **toupper** |
| 0003deee | FUN_0003deee | 77 | **strnicmp** |
| 0003dfcf | FUN_0003dfcf | 25 | **cenvarg** (env/arg build) |
| 0003e361 | FUN_0003e361 | 32 | **makepath** |
| 0003e7f7 | FUN_0003e7f7 | 30 | **strchr** |

## Confirmed library — module name best-effort (40–89% coverage)

| addr | FUN | size | CLIB source module |
|---|---|---|---|
| 0003be40 | FUN_0003be40 | 83 | prtf (printf core) |
| 0003a97c | FUN_0003a97c | 73 | tell |
| 0003d894 | FUN_0003d894 | 119 | ioalloc |
| 0003a93b | FUN_0003a93b | 65 | lseek |
| 0003b8cd | FUN_0003b8cd | 43 | fopen (helper) |
| 0003d3e4 | FUN_0003d3e4 | 43 | fgetc |
| 0003dcb5 | FUN_0003dcb5 | 48 | ltoa |
| 0003dbeb | FUN_0003dbeb | 48 | ltoa |
| 0003d40f | FUN_0003d40f | 171 | fgetc (__fill_buffer) |
| 0003d36b | FUN_0003d36b | 121 | fgetc |
| 0003a526 | FUN_0003a526 | 83 | atol |
| 0003da03 | FUN_0003da03 | 52 | ftell |
| 0003d935 | FUN_0003d935 | 49 | qread |
| 0003b539 | FUN_0003b539 | 91 | fread |
| 0003b239 | FUN_0003b239 | 58 | d_getvec (DOS vector) |
| 0003db36 | FUN_0003db36 | 45 | chktty |
| 0003a579 | FUN_0003a579 | 31 | open |
| 0003b7e8 | FUN_0003b7e8 | 229 | fopen (core) |
| 0003b273 | FUN_0003b273 | 55 | d_setvec (DOS vector) |
| 0003e590 | FUN_0003e590 | 36 | unlink |
| 0003b99e | FUN_0003b99e | 44 | fclose (helper) |

## Probably library — low confidence (15–40%, verify)

ad66→printf, cc45→spawnve, c42d→prtf, b407→rewind, c479→dosret, af38→system,
adb2→int386, cfce→spawnlp, c491→dosret, c002→prtf, dae1→allocfp, b90d→fopen, a4fa→sprintf.
(These call/return into library routines; the low coverage means the function body is mostly
game glue or is heavily relocated. `cc45`, `b407`, `cfce`, `c002` are already byte-matched as
game wrappers with `-3s -of`.)

## Game's own framed code (<15% — NOT in any library)

These are thin game wrappers that call the library; they match our compiler (`-3s -of`):
db69, aee6, aa74, ab59, ab69, ad89, c57b, b2aa (INP port read), ca0d (return 0),
b9ca, addb, b8f8, e48e, e471, b2b5. (Matched ones: db69, aee6, aa74, ab59, addb, b8f8, e48e, e471.)
