# Syndicate Matching-Decompilation, Working Memory

> **Read this first when starting a session in this repo (`C:\Users\James\freesynd`).**
> It is the persistent memory for the decomp effort. Keep it up to date: when a
> decision is made or a milestone lands, edit the **Current Status** and **Session
> Log** sections. This is intentionally NOT stored in Claude's global memory (other
> agents run on this machine), it lives in the repo.

> **⚠ REPO = `vibesynd` (2026-07-11).** This decomp is the repo root
> (`C:\Users\James\freesynd`) and its GitHub remote is now
> **`github.com/WildPress/vibesynd`** (main = decomp, private). The old
> FreeSynd/VibeSynd *port* that used to sit in `legacy/` has been removed from this
> machine, its committed history is parked on the remote branch
> **`freesynd-port-archive`** (safe to delete once no longer wanted). This project
> no longer has any FreeSynd relation. **Any `reverse/` prefix in older notes below
> means the repo root** (old `reverse/tools/` is now `tools/`, etc.), WSL path is
> `/mnt/c/Users/James/freesynd`.

## Goal

Produce a **matching decompilation** of the original 1993/95 DOS *Syndicate*
executable: C source that, compiled with the period toolchain, produces
**byte-identical machine code** to the original binary, one function at a time.

This is a *different discipline* from a behavioural reimplementation (the abandoned
FreeSynd-style port). Here the **original binary's assembly is the source of truth**,
and success = zero-diff parity, tracked in a coverage manifest.
Video/DOSBox footage is for **orientation only** (understanding what a function is
for, prioritising), never the parity signal.

## Confirmed facts about the target

- **Compiler: Watcom C/C++ (~9.5 / 10.0).** The binary carries
  `Copyright by WATCOM ... 1988-1993` and multiple `WATCOM` runtime banners.
- **Calling convention: MIXED, determine per-function from the disassembly.**
  ⚠ The old "register calling" note was from OUR OWN smoke test (proving OW v2's
  *default*), NOT the original. Empirically the original binary has **stack-calling**
  functions: `FUN_0003b9ee` (nibble→hex), `FUN_0003dce5` (toupper), `FUN_0003da37`
  (tolower), `FUN_0003dfcf` (strcpy) all take params at **`[ebp+8]`** (stack), and
  match `-5s` (stack calling) byte-for-byte in prologue+body. Others pass args in
  `eax/edx` (register). **Read the param source:** `[ebp+8]`/`[esp+4]` ⇒ compile `-5s`,
  arg in `eax` ⇒ `-5r`. (How the binary mixes these, per-unit flags or `#pragma aux`, 
  still TBD.)
- **Format: Linear Executable (`.LE`)**, 32-bit protected mode, flat memory model,
  **DOS/4GW** extender. `SYNDICAT_MAIN.EXE` is 518,713 bytes, dated **Feb 1995**.
- **Segments already extracted** (in `inputs/`, git-ignored):
  - `SYNDICAT_MAIN_OBJECT1.bin`, 261,620 B, **code**
  - `SYNDICAT_MAIN_OBJECT2.bin`, 53,248 B, data
  - `SYNDICAT_MAIN_OBJECT4.bin`, 114,688 B, data/BSS
  - `.linear.bin` variants are the un-relocated/linearised copies.

## Architecture: container core + native host tools

| Piece | Where | Role |
|---|---|---|
| Open Watcom (`wcc386`, `wpp386`, `wdis`, `wlink`, `wlib`) | 🐳 container `synd-decomp` | Deterministic codegen (parity-critical) |
| objdiff-cli / byte compare | 🐳 container | Match oracle |
| decomp-permuter | 🐳 container | Auto-search C variants near a match |
| Harness (`tools/`) | 🐳 container | Compile + diff loop, manifest |
| Ghidra GUI + **GhidraMCP** (LaurieWired) | 🪟 Windows host | Interactive RE, Claude drives, user watches |
| Ghidra headless | 🪟 Windows host | Batch function inventory |
| DOSBox-X | 🪟 Windows host | Review footage / dynamic tracing (orientation only) |

**Match oracle:** the ultimate check is **byte equality** of a function's machine
code. `wdis` emits raw bytes + disassembly for our compiled `.obj`, the original
bytes come from `OBJECT1.bin` (via Ghidra). Compare bytes, use disassembly to read
the diff. This sidesteps cross-disassembler syntax mismatch.

## Environment specifics (this machine)

- **Host:** Windows 11. This Claude Code session runs on the Windows host. Its Bash
  tool is **Git Bash** (no docker). **All container/Linux work must go through WSL:**
  `wsl -e bash -lc '...'` (from PowerShell) or the Bash tool calling `wsl`.
- **Docker:** native **Docker CE inside WSL2 Ubuntu** (Docker 29.x). **No Docker
  Desktop.** User is in the `docker` group (no sudo). Daemon runs. ~530 GB free in WSL.
- **Image:** `synd-decomp` (built from `docker/Dockerfile`).
- **Cached downloads (WSL home `~/synd-decomp/`):** `ow-snapshot.tar.xz` (~151 MB, the
  Open Watcom tree), `owinst` (the TUI installer, **unused**, see below).
- **Repo path from WSL:** `/mnt/c/Users/James/freesynd`.

### Open Watcom install method (IMPORTANT)

Do **NOT** use the `open-watcom-2_0-c-linux-x64` TUI installer, it SIGFPEs / hangs
headless in Docker. Instead extract the plain snapshot archive
`ow-snapshot.tar.xz` (asset on the `Current-build` tag). It unpacks to a standard
tree: `binl64/` (Linux x64 host binaries), `h/` (INCLUDE), `lib386/dos/` (DOS 32-bit
libs). The Dockerfile does this. `Current-build` is a rolling tag, pin to a dated
build + checksum if exact toolchain reproducibility ever matters for a match.

## Visibility tooling

- **Decomp dashboard (LOCAL only, not claude.ai)**, `tools/dashboard.py` (runs
  in-container) writes `dashboard/data.json` + `dashboard/index.html`: coverage meter,
  all functions (sortable/filterable), click-through per-function disassembly, live
  status panel. The page fetches `data.json` and auto-refreshes every 5s. Serve + open
  <http://localhost:8777/>:
  `python -m http.server 8777 --bind 127.0.0.1 --directory dashboard` (Windows Python).
  Regenerate after matches:
  `docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/dashboard.py`.
  (User asked to keep this fully local, do NOT publish it as a claude.ai Artifact.)
- **GhidraMCP (bethington/ghidra-mcp v5.14.2)**, chosen over LaurieWired's (which was
  pinned to Ghidra 11.3.2), bethington's is built for **12.1.2** exactly, 251 tools,
  actively maintained. Extension installed at
  `C:\ghidra\ghidra_12.1.2_PUBLIC\Ghidra\Extensions\GhidraMCP\`. Bridge at
  `tools/bridge_mcp_ghidra.py`. Registered as an MCP server in `.mcp.json`
  (repo root) using **stdio** transport, **no `--lazy`** (their README says lazy is not
  recommended for Claude Code). Deps: `mcp>=1.28.0`, `requests>=2.32.0` in the Windows
  Python. **To use:** restart Claude Code (approve the `ghidra` MCP server), then in
  the Ghidra GUI open project `SyndDecomp` → the OBJECT1 program, and enable the plugin
  (File > Configure > GhidraMCP). The bridge auto-discovers the running Ghidra, its
  tools only work while Ghidra GUI is open with a program loaded.

## Commands  (CURRENT, period Watcom 10.0a via DOSBox is the matching compiler)

```bash
# All run from WSL:  wsl -e bash -lc '...'   (repo mounts at /work in the container)
cd /mnt/c/Users/James/freesynd
D() { docker run --rm -v "$PWD":/work -w /work synd-decomp "$@"; }   # convenience

# MATCH one function: compile src/<name>.c with Watcom 10.0a + relocation-aware diff.
D bash tools/match10.sh <name> "-4s -oneatx -zp8 -s -zq"   # -4s=stack / -4r=register
D python3 tools/mark.py <name>          # once "RELOC-AWARE match (masked): YES"

# PICK targets:
D python3 tools/pick.py 40 1            # unmatched size<=40 calls<=1 + matched count
D python3 tools/smallest.py            # smallest non-framed unmatched + first bytes
D python3 tools/triage.py             # unmatched grouped by byte signature

# BATCH simple patterns (auto-compile + reloc-diff + mark):
D python3 tools/batch_match.py         # ret-const(b8..c3) / getter(a1) / setter(a3)
D python3 tools/batch_stubs.py         # mov eax,imm; jmp  dispatch-stub cluster

# Rebuild image (DOSBox baked in) / regen dashboard:
docker build -t synd-decomp docker
D python3 tools/dashboard.py
```

**Confirmed compiler + flags.** Matching compiler = **period Watcom C/C++ 10.0a**
(DOS `WCC386.EXE` via `W32RUN`, headless under DOSBox, `tools/wcc_dos.sh`, tree in
`toolchain/watcom10a/`, git-ignored). **NOT Open Watcom v2**, its register allocator
diverges (EAX vs EDX). Base flags **`-4s`** (stack) or **`-4r`** (register) + `-oneatx
-zp8 -s -zq`. Choose per function: params at `[esp+N]`/`[ebp+N]` ⇒ `-4s`; args in
`eax,edx` ⇒ `-4r`. Game is `-4` (486), `-5` uses a Pentium `mov+sar` the original lacks.
(`tools/wcompile.sh` = OW v2, only for CPU-independent smoke tests now.)

## Per-function matching loop  (CURRENT)

1. Pick an **unmatched, non-framed** target, SKIP `55 89 e5` prologues (blocked class).
2. `disassemble_function` (Ghidra MCP) → read shape, note param source (stack vs reg).
3. Write matching C → `src/<name>.c`. Idioms that matter:
   - global access via `a1`/`a3`/`803d` (simple abs load/store) ⇒ `extern` var, the differ
     masks the 00000000 reloc placeholder. Works because the addend is 0.
   - obj1-internal DATA at a computed position (array bounds, `arr[const]`,
     `((T*)addr)->field`, loop `cmp ebx,end`) relocates with a NON-ZERO addend (e.g.
     `00 1e 00 00` = 0x1e00). ✅ **FIXED (cont. 9): the differ now masks real OMF FIXUPP sites**
     (`tools/omf.py` parses the .obj; `match_reloc.py` masks exactly those byte ranges), so the
     plain **`extern` form works** for these (keeps the bounds guard the original has). This
     unblocked 0x22b38. Prefer `extern` (differ handles the reloc). A literal-address cast
     `((struct T*)0x15e70)` is an alternative that gives EXACT bytes, but a literal loop bound
     is compile-time-provable so Watcom may DROP the entry guard — use `extern` unless you have
     a reason not to.
   - `return h(imm)` register-calling ⇒ Watcom tail-calls to `jmp` (matches thunk stubs).
   - call/jmp/abs-data operands are RELOCATIONS, the differ masks them, declare callees
     & globals `extern` and they won't block a match.
   - **Register allocation is the main game-code wall.** Forced allocation (one sensible
     register per value: forwarders, initialisers, linear call-seqs, setters) matches. Free
     allocation (2+ values each fitting several registers) lets Watcom diverge from the
     original's choice — hard to force from C. Target the forced-allocation shapes.
4. `tools/match10.sh <name> "<flags>"` → want `RELOC-AWARE match (masked): YES`. If close,
   flip `-4s`/`-4r` or adjust the C, a couple tries, then move on (don't rabbit-hole).
5. `tools/mark.py <name>` to record. Prefer batchable shapes (stubs, getters/setters,
   forwarders, call-sequences) for volume, the ~275 `push ebx` fns are real logic, 1-by-1.

## Directory layout

```
reverse/
  AGENTS.md        <- this file (session memory)
  README.md        overview
  inputs/          original binary + segments        (git-ignored, copyrighted)
  ghidra/          Ghidra projects / databases        (git-ignored)
  docker/Dockerfile pipeline image definition
  run.sh           host wrapper (WSL): docker run into the image
  tools/           harness: wcompile.sh (+ match/diff scripts to come)
  src/             our written matched C               (smoke.c is a throwaway test)
  asm/             extracted target disassembly per function
  manifest/        functions.json (inventory + status) + README (schema)
  build/           compiled objects                    (git-ignored)
```

## Legal / hygiene

Standard decomp discipline. The original executable, extracted segments
(`inputs/`), and Ghidra databases (`ghidra/`) are **git-ignored and never
committed**. Only tooling, our written C, and the manifest are tracked.

## Findings from the binary (calibration data)

From the first Ghidra headless analysis of `OBJECT1.linear.bin` (base 0x0):

- **357 functions** identified (`manifest/functions.json`). Only **14 are pure
  leaves** (no calls, no data refs).
- **Frame pointer is OMITTED** in the mainstream case: only **34/357** start with
  `push ebp; mov ebp,esp` (`55 89 e5`). The other 323 use `push ebx/esi/edi` +
  esp-relative locals. So do NOT add `-of` globally, most functions are frameless.
  The 34 frame functions are the exception (match them with frame flags case-by-case).
- **Register calling convention** confirmed (args eax, edx, ebx, ecx).
- **The pure-leaf functions are low-level runtime/asm primitives, NOT clean C:**
  byte-swap (`lods; xchg al,ah; rol`), Watcom-unrolled dword fill, a segmented-memory
  heap allocator (`mov ds,edx`), jump-table switch thunks, register/segment context
  restore. Several use **custom register conventions** (e.g. count in ecx) that would
  need `#pragma aux` to reproduce. These make poor first targets.
- **Implication for first match:** target a small **non-leaf game-logic function**
  with the standard convention, and use objdiff's **relocation handling** (declare
  callees/globals as externs, the diff ignores the specific relocated address so
  calls/data refs don't block a match). "Pure leaf" was an over-constraint.

## Current Status  (update every session)

### ⭐ SNAPSHOT, read this first (as of 2026-07-11)
- **Coverage: 57/500 matched** (byte-identical, relocation-aware). See `manifest/functions.json`.
- 🔓 **LOOP-ROTATION WALL CRACKED (cont. 11): `0x377b8` matched with `-4s -or -s -zq`.** A
  chain-length counter (while-loop over a linked list). The main-game `-oneatx` bundle ROTATES
  loops (test at bottom); the original has the UN-rotated form (test at top, `jmp` back). This
  unit wants **lighter opt** (like the `0x39xxx` block). Three levers combined: (1) drop `-oneatx`
  → `-or` alone gives top-test + fixes the reg swap (id→EAX, count→EDX) + gets the inc/mov
  schedule; (2) an explicit pointer local (`node = base + id`) keeps the address in two steps
  (`add`+`mov`, not a folded `[reg+disp]`); (3) the original's redundant entry-guard test comes
  from an **`if (cond) while (cond) {}`** shape the light optimiser doesn't fold. Recipe recorded
  in `recipes.json`; regression 49/49. ⇒ **loop rotation IS steerable** (opt level + loop form),
  unlike the pure register tie-break in `0x33fb8`.
- 🧱 **`0x33fb8` (map passability) parked at 99%**, one byte off, a pure register tie-break
  (base in EDX+`lea` vs EBX+`add`). Tried 7 C reformulations, none flip it. Deterministic in
  Watcom but not reachable from portable C. Documented in `docs/register-allocation.md` (worked
  example) + journal. This is the genuinely-hard class; don't rabbit-hole it.
- ⚠ **`0x36168` outlier: saves EBX without using it** (push ebx / … / pop ebx wrapping a body
  that never touches ebx). `-4s` gives the exact body but no wrapper; `-4r` tail-calls. Likely an
  optimised-away local in the original source. Parked as a curiosity, not representative.
- 🔴🎯 **COMPILER = Watcom 9.5b, NOT 10.0a (cont. 10 — CORRECTS cont. 8).** Use
  **`tools/wcc_95.sh` / `tools/match95.sh`** as the PRIMARY compiler. Same flags: `-4s`/`-4r`
  `-oneatx -zp8 -s -zq` (per-function convention). **Proof:** 9.5 matches ALL 48 regression
  recipes (10.0a's matches hold) AND fixes a class 10.0a can't: **byte bit-tests**
  (`if (mem & bit)`). 10.0a always emits `mov r8,mem; and r8,imm; and r32,0xff` (load+mask); the
  original uses `test byte[mem],imm`, and **only 9.5 emits that at `-4s -oneatx`**. NB: 10.0a
  `-os`/`-3` DO emit `test byte[mem]` in isolation, BUT neither matches the FULL 0x2d998 (they
  change other codegen) — tested `-4s`/`-3s`/`-os` on 10.0a, ALL fail; only 9.5 `-4s` matches.
  So it's the compiler VERSION, not a flag. 0x2d998 (a flag-state fn) matched 9.5 EXACT. cont.8 wrongly
  "disproved" 9.5 — but that was on framed LIBRARY fns with unrelated differences (push order),
  not clean game code. **Bottom line: switch to 9.5; it unlocks flag/bit-field logic everywhere.**
  10.0a (`wcc_dos.sh`/`match10.sh`) stays available for A/B, but 9.5 is the game's compiler.
  Full workflow + commands in the **Commands** and **Per-function matching loop** sections above.
- 🎯 **FLAGS CALIBRATED (cont. 9) against a regression baseline** (`manifest/recipes.json` =
  flags for all 48 matches, built by `tools/recipes.py`; `tools/caltest.py` substitutes a
  candidate opt/packing into every recipe and reports how many still match). Results:
  **`-oneatx` is PINNED** (keeps 48/48; `-ox` breaks 3, `-ot` 8, `-os` 17, so alternatives are
  ruled out, not just untested). CPU **`-4`** (movsx in 0x26ba8 kills `-5`). Convention per-fn
  (`-4s`/`-4r`/`-3s -of`, read from disasm). Stack-check off (`-s`, no probes seen). **PACKING is
  NOT pinned: `-zp8`/`-zp4`/`-zp1` all keep 48/48** (no current match has a packing-sensitive
  struct). ⇒ when matching a struct-heavy fn, TRY packing variants (it's the one open flag);
  a struct fn that discriminates them would pin it. **Discipline: on GAME code a miss is the C,
  not the flags (flags are constrained by the regression set) — vary the C, don't flag-hunt.**
- 🔑 **FRAMED CLASS UNLOCKED (2026-07-11 cont. 7): use `-3s -of` (NOT `-of+`).** The old
  "framed lean-epilogue is blocked" note was a FLAG ERROR, earlier sweeps tested `-of+` (adds
  `89 ec` = mov esp,ebp) but never plain **`-of`**. Plain `-of` forces a frame with the LEAN
  epilogue (`pop ebp; ret`, no `mov esp,ebp`), EXACTLY the original. Combined with **`-3`** CPU
  level (which emits the direct memory push `ff 75 08` instead of `-4/-5`'s `mov edx,[..]; push`),
  `-3s -of -oneatx -zp8 -s -zq` matches the whole **framed forwarder / call-sequence class**
  byte-for-byte. Banked 12 this way (0x3db69, 3aee6, 3aa74, 3ab59, 3addb, 3b8f8, 3cfce, 3e48e,
  3c002 [indirect `call [mem]`], 3e471, 3cc45, 3b8cd, 3b972). Recipe details: forwarders w/
  globals ⇒ `extern int g;` + `f(...,g)`, indirect ⇒ `extern void(*fp)(); fp(...)`, `&param` ⇒
  `lea eax,[ebp+N]; push eax`. **`-of` skips LEAF functions** (no frame emitted), so leaf-framed
  stays blocked (see below).
- **Diff = relocation-aware** (`tools/match_reloc.py` masks call/jmp/abs-data operands, declare
  callees/globals `extern`). `RELOC-AWARE match (masked): YES` = a match.
- **All infra up:** DOSBox baked into the `synd-decomp` image, toolchain in `toolchain/`
  (git-ignored, provenance in `toolchain/README.md`), GhidraMCP + dashboard working.
  `tools/framed.py` lists framed (`55 89 e5`) unmatched fns w/ first bytes, `tools/wcc_la.sh`
  compiles with the 10.0 LA compiler (from `toolchain/la_stage`) for A/B version tests.
- **To RESUME a session:** launch Ghidra GUI (`ghidraRun.bat`, JAVA_HOME = JDK 21) → open
  project `SyndDecomp` → OBJECT1 program → enable GhidraMCP (File > Configure > **Utility**).
  Then `list_instances` → `connect_instance SyndDecomp`, and run the loop in **Commands**.
- ✅ **FRAMELESS class matches RELIABLY with 10.0a** (`tools/byprefix.py 53` = 272 push-ebx
  logic fns, the bulk). Recipe: **`-4s`** (stack args at `[esp+N]`) or **`-4r`** (register) +
  `-oneatx -zp8 -s -zq`, **NO `-of`**. The `push ebx` emerges from Watcom's own register
  allocation when the C keeps a value live, don't force it. 5/5 EXACT (not just masked) first-try
  this session: 0x34168 (clamp-decrement byte field), 0x25378 (ptr-stride loop), 0x18488
  (global-sub + call), 0x26aa8 (max of two abs), plus reg cases. Just get the C logic + integer
  types right (uchar/ushort widths drive the `movzx`/`xor+mov` zero-extends).
- **Next work:** bank frameless fns (`tools/byprefix.py 53`, `-4s`/`-4r`), reliable volume, and
  framed forwarders (`-3s -of`, `tools/framed.py`). Skip switch-table dispatchers (`jmp cs:[reg*4+
  tbl]`) and variadic (`va_arg` = `lea,add,mov [x-4]`) for now.
  🟢 **9.5 HYPOTHESIS DISPROVEN + framed class RE-EXPLAINED (cont. 8).** Downloaded & stood up
  **Watcom 9.5b** (`tools/wcc_95.sh`, `tools/match95.sh`, tree `toolchain/watcom95/BIN`, git-ignored,
  provenance in cont. 8). Tested it on the framed-logic failures: **9.5 emits byte-identical output
  to 10.0a** (same `55 89 e5 53` save order, same `89ec5d` not `leave`, same `cmp mem` not
  eager-load, swept ~11 flags, none flip it). So the framed-logic mismatch is **NOT a Watcom
  version difference.** ⇒ **DO NOT chase older Watcoms for it.** ✅ **The real explanation: EVERY
  framed fn lives in `0x3a000-0x3e600`** (the top slice of the code seg), everything below is
  frameless game code. That top slice is the **linked-in Watcom C RUNTIME LIBRARY + DOS/4GW support**
  (INT21 file I/O, byteswap, strcpy, getc/putc, malloc-ish, the funcs we kept hitting). It was
  **precompiled by Watcom** with library build settings we don't control, hence the different
  codegen (save-order, single shared epilogue, `leave`, leaf frames). **Right approach for these:
  byte-match against the actual CLIB `.obj` (we have 9.5 `CLIB3R/S.DOS` + 10.0a's), NOT recompile
  from C**, or just DEPRIORITIZE them (they're runtime, not game logic). The framed FORWARDERS in
  that region matched via `-3s -of` only because forwarders have zero codegen choices.
  ✅ **PROVEN by byte-search (`tools/libmatch.py`), not just clustering:** slid a 12-byte window
  over each framed fn and searched the concatenated 9.5 + 10.0a `CLIB3R/S.LIB` blob. **~37 framed
  fns have HIGH coverage (≥40%, most 100%), their exact compiled bytes ARE in the Watcom C
  library** ⇒ they are library code (e.g. 0x3da37=`tolower` [`'A'..'Z'`+0x20], 0x3dce5=`toupper`
  [`'a'..'z'`−0x20], 0x3aed8=`abs`, 0x3d3e4/0x3d40f=`fgetc`/`__fill_buffer`, 0x3dfcf, 0x3aef9,
  0x3b7e8, 0x3a8d7…). The **LOW-coverage (0%) framed fns are GAME wrappers** that CALL the library
  (0x3db69, 0x3aee6, 0x3aa74, 0x3ab59, 0x3ab69, 0x3c57b, 0x3ad89), which is exactly why THOSE
  matched our compiler with `-3s -of`. So the framed block is a MIX, split cleanly by libmatch
  coverage. **Correction to cont.5's note:** 0x3dfcf is NOT "custom, not CLIB", it IS library
  (100% coverage), cont.5 only compared it to the `strcpy` module, not the whole lib.
  ⇒ **For library fns: don't write C, tag/skip them (identify via `libmatch.py` + the lib symbol
  table), match by linking the real lib object if ever needed. Focus decomp on the FRAMELESS GAME
  CODE** (0x10000-0x39xxx, `tools/byprefix.py 53`, `-4s`/`-4r`), the actual game, matches reliably.
  Asm primitives (IN/OUT/INT21/rep) in the same region are hand-asm. Full journey: Session Log 1–8.

**Phase 1, reproducible container core: DONE ✅**
- `synd-decomp` image builds, `wcc386` runs, compile→`wdis` loop proven on `smoke.c`.

**Phase 2, RE workbench + inventory: MOSTLY DONE ✅**
- ✅ JDK 21 (Ghidra needs it, JDK 25 was already installed but unsupported by Ghidra 12).
- ✅ Ghidra 12.1.2 installed at `C:\ghidra\ghidra_12.1.2_PUBLIC`.
- ✅ Headless inventory: `SeedDisassemble.java` (pre) + `ExportFunctions.java` (post)
      → `manifest/functions.json` (357 fns). Project `SyndDecomp` saved in `ghidra/`.
      Re-run: see the analyzeHeadless command in **Commands** below (adapt paths).
- ⏸️ **GhidraMCP NOT installed yet**, deferred to the interactive matching phase.
      Registering it as an MCP server **requires a Claude Code restart**.
- [ ] #7 DOSBox-X (review footage, low priority).

**Phase 3, matching (IN PROGRESS)**
- ✅ `tools/match.py` built AND proven end-to-end on a real function.
- ✅ **FIRST MATCH: `FUN_0003c46d` @ 0x3c46d, 100% byte-identical.**
      `void *f(void){ return (void*)0x11e3c; }` → `B8 3C 1E 01 00 C3` under the
      default `WATFLAGS` (`-6r -oneatx -zp8 -s -zq`). Frameless register-convention
      accessor (returns a global's addr, called from 20 sites). Source in
      `src/FUN_0003c46d.c`, manifest marked `matched`. **1/500.**
- 🐞 **FIXED: manifest `image_base` was `00000000` but the real base is `00010000`**
      (Ghidra/LX maps OBJECT1 code at 0x10000, file offset = ghidra_addr − 0x10000).
      `match.py` reads `off = addr − image_base`, so it was reading garbage target
      bytes until this was corrected. **Any tool that maps manifest addr → file offset
      must subtract 0x10000.** (`get_metadata` still *reports* base 0, ignore it, the
      LX memory block sits at 0x10000. Empirically verified: our exact bytes live at
      file 0x2c46d for ghidra addr 0x3c46d.)
- 🔑 **RELOCATION RESOLVER WORKING: `tools/le_fixups.py`** parses the LE fixup
      table from `inputs/SYNDICAT_MAIN.EXE` (MZ e_lfanew=0x28b8 → LE header, fixup
      **page** table off = header+0x68, **record** table off = header+0x6c, note the
      `dircnt` dword at +0x64 that's easy to skip). Parses all **10,060** fixups,
      object table confirmed (obj1@0x10000 sz0x3fdf4, obj2@0x50000, obj3@0x70000 BSS
      0 pages, obj4@0x80000). Validated: switch-table disp at 0x11e40 resolves to
      obj1:+0x1fc2. **Usage:** `python3 tools/le_fixups.py [src_addr_hex]`.
- ✏️ **CORRECTION:** `0x11e3c` is **NOT a relocation** (earlier note was wrong). It's
      an absolute pointer to a global uint, baked as a literal, DOS/4GW loads obj1 at
      a fixed base so internal addrs need no fixup. Proven by callers: `*(uint*)
      FUN_0003c46d() = val` (e.g. FUN_0003c4b9, which is key-translation code). So
      `FUN_0003c46d`/`FUN_0003c473` are `return &global;` accessors for input state.
      **Code/data caveat:** 0x11e3c & 0x11e40 are **DATA** (writable globals). Proven by
      the caller's raw asm: `CALL 0x3c46d; MOV [EAX],EBX`, it *writes* to 0x11e3c.
      Ghidra's disassembly of that address as `JMP CS:[EAX*4+..]` (and the giant
      FUN_00011d68) is a **mis-analysis of data as code**, the classic un-relocated-image
      pitfall. The fixup at 0x11e40 is that global's initial value (a reloc'd pointer to
      0x11fc2), later overwritten at runtime. Exact code/data boundaries in obj1's low
      region need full relocation + re-analysis before trusting Ghidra's carving there.
      (Earlier note claiming this region "IS real code" was WRONG, corrected.)
- ✅ `FUN_0003c473` (sibling accessor, +4 global) matched, **2/500**.
- ⛔ `FUN_0003ca0d` (`int f(void){return 0;}` **framed**, target `55 89 e5 31 c0 5d c3`)
      **BLOCKED on toolchain.** OW v2 gets `31 c0 c3` frameless by default, `-of+`
      forces a frame but emits `55 89 e5 31 c0 **89 ec** 5d c3`, one extra `mov esp,ebp`
      (redundant esp restore) that the original omits. Swept CPU level (-3r/-4r/-5r/-6r),
      opt bundles (-os/-oh/-oxs/-ot/-oi/-oe), stack-check on/off, none trim it. `-of`
      always drops the frame, `-of+` always adds the esp restore.
      ✏️ **UPDATE (hypothesis was WRONG):** got period **Watcom 10.0a** running (below) and
      it produces the **identical** bytes to OW v2 here, default `31 c0 c3`, `-of+`
      `55 89 e5 31 c0 89 ec 5d c3`. So the lean epilogue is **NOT** an OW-v2-vs-period
      difference, both agree. The original's lean `...31 c0 5d c3` comes from something
      else. Extended flag sweep (`-d1`, `-ee`, `-et`, opt bundles, CPU levels, on BOTH
      OW v2 and 10.0a), **nothing** produces the lean epilogue. NOT 9.5 (banner is
      `C/C++32` = 10.0 branding, not 9.5's `C/386`). **Conclusion: likely HAND-WRITTEN
      ASM** (a lean-frame `return 0` stub, like this binary's other asm runtime
      primitives). Match path if we bother: write `.asm`, assemble with `WASM.EXE`.
      No prior art, no existing Syndicate-1993 matching decomp exists (only the
      Syndicate *Wars* port).
      ✅ **RESOLVED (mostly):** NOT hand-asm. It's a **stack-calling framed** fn. With
      `-5s -of+`, OW v2/10.0a emit `55 89 e5 31 c0 **89 ec** 5d c3` vs target
      `55 89 e5 31 c0 5d c3`, off by exactly one `mov esp,ebp`. SAME single-instruction
      gap on `FUN_0003b9ee` (whose prologue+body match `-5s -of+` byte-exactly). So the
      whole **stack-calling framed class** is one epilogue instruction away.
- ❌ **CLIB hypothesis DISPROVEN.** Extracted real `strcpy` from 10.0a `CLIB3S.LIB`
      (`wlib lib :module=out.obj`, note OW v2 extract op is `:` not `*`). It's a
      DIFFERENT impl: frameless, unrolled 2B/iter, returns original dest (std strcpy).
      The game's `FUN_0003dfcf` is framed, 1B loop, returns dest+len (non-std). Game
      `toupper` is a range-check, Watcom's is a table lookup. **⇒ these framed functions
      are CUSTOM GAME CODE, not CLIB.** (CLIB3R/CLIB3S both exist in tree, but the game's
      utility fns aren't them.)
      CLIB3S strcpy = `8b4424048b542408508a0a880880f90074118a4a0183c20288480183c00280f90075e658c3`
- 🧱 **REAL BLOCKER (confirmed across 4 fns: return-0, nibble→hex, strcpy, toupper):**
      the game's **framed stack-calling** fns have a **lean epilogue** (`pop ebp; ret`,
      no `mov esp,ebp`). **Neither OW v2 NOR 10.0a produces this**, `-5s -of+` matches
      prologue+body byte-exact but always appends `89 ec`. This is a genuine Watcom
      **version** gap (not flags, not CLIB, not asm, all ruled out). Untried versions:
      **release 10.0** (not beta/not 10.0a), **9.5**, **10.5**. Bounded task for later.
- ✅ **FUN_00018878 MATCHED (3/500)**, first non-leaf. Stack-calling forwarder,
      compiled with **Watcom 10.0a `-5s`**, **relocation-aware** (call target masked).
      Tool: `tools/match_reloc.py` (masks E8/E9 rel32 in target+ours, honors $WATFLAGS
      and SKIP_COMPILE). This proved 10.0a is the right compiler (see below).
- ✅ **What works NOW:** frameless fns (register OR stack calling) match with **Watcom
      10.0a** + relocation-aware diffing. Framed lean-epilogue class still blocked (10.0a
      also emits the `89 ec`). Strategy: bank frameless fns via 10.0a, framed class later.
- ❌ **VERSION THEORY DISPROVEN for the framed lean epilogue.** The "10.0 beta" is really
      **"WATCOM C/C++ 10.0 Limited Availability"** (a real pre-GA release, Mar-1994, its
      README + installer copyright `1988-1993` MATCHES the game's runtime banner, so the
      game likely used *early* 10.0). Extracted its `wcc386.exe` from the packed install
      (`WPACK.EXE c:\PACK0022` etc. under DOSBox → `toolchain/la_stage/`). Tested: LA 10.0
      **also emits `89 ec`** on `-5s -of+` (return-0 → `5589e531c089ec5dc3`, same as 10.0a/OWv2).
      **⇒ ALL 10.0-family compilers emit it, the lean epilogue is NOT a version choice.**
- 🧩 **Sharper framed puzzle (parked):** game used early 10.0 (copyright match), which emits
      `89 ec` with `-of+`, yet the game's framed fns are LEAN. So their frame does NOT come
      from `-of+`. It must come from the **C source genuinely needing `ebp`** (addr-of-param/
      local, alloca, or similar) → lean epilogue naturally. Next attempt (if/when): find the
      *source construct* that makes Watcom emit a needed (not forced) lean frame, NOT a flag
      or a version. Until then: match the ~90% frameless fns with 10.0a.
- WPACK note: `wpack <packfile>` (no filelist) extracts all, runs fine under DOSBox.
      `la_stage/` holds the extracted LA `wcc386/w32run/dos4gw` (git-ignored, re-derivable).
- [ ] Next: keep banking **frameless** register-convention targets, then non-leaf game
      logic w/ objdiff relocation handling. Framed-epilogue puzzle parked.

### Verify-later (dynamic), questions static analysis can't fully settle
Debugger deferred (paths may need deep game progress to hit). When we're in the
debugger for any reason, opportunistically check these:
- **0x11e3c / 0x11e40**: believed to be writable globals (last-key / last-translated-char
  state) accessed via FUN_0003c46d/FUN_0003c473. They sit *inside* real code
  (FUN_00011d68 is genuinely called from 0x2e91a/0x2e76e), so obj1 is code+data mixed
  and Ghidra's carving there is suspect. Watchpoint on 0x11e3c while pressing keys →
  confirm it's data + see live values. Confirm nothing executes 0x11e3c.

### TODO / known issues
- decomp-permuter `requirements.txt` path wrong in Dockerfile (install is `|| true`,
  no-ops). Fix when the permuter is first needed.
- Watcom `WATFLAGS` unverified until the first real match calibrates them.
- **Period Watcom 10.0a IS NOW AVAILABLE.** Downloaded from WinWorldPC (ISO),
  extracted, staged at `toolchain/watcom10a/WATCOM` (git-ignored via `toolchain/`).
  Runs headless under **DOSBox** in the container via **`tools/wcc_dos.sh <name>`**
  (dosbox self-installs on first run). Gotchas learned: (1) `BINB/WCC386.EXE` is a Win32
  app needing **`W32RUN.EXE`** → PATH must include `C:\WATCOM\BIN` (has W32RUN + DOS4GW).
  (2) DOSBox shell has no `2>&1`. (3) DOS 8.3 names → copy source to `SRC.C`.
  Compiler banner: "WATCOM C32 Optimizing Compiler Version 10.0a".
- 🔴 **OW v2 DIVERGES from period 10.0a, use 10.0a as the PRIMARY compiler.**
  (Earlier "identical output" note was WRONG, only true for allocation-free fns.)
  On `FUN_00018878` (stack-calling forwarder), OW v2 emits `mov EAX,[esp+4]; push EAX`,
  Watcom 10.0a emits `mov EDX,[esp+4]; push EDX`, and **10.0a matches the original**.
  Their register allocators differ. Our first 2 matches passed on OW v2 only because
  `mov eax,imm; ret` has no allocation choices. **⇒ compile matches with
  `tools/wcc_dos.sh` (10.0a via DOSBox), NOT wcompile.sh (OW v2).** TODO: make match.py
  call wcc_dos.sh by default (add SKIP_COMPILE plumbing, `match_reloc.py` already has it).
- **CPU LEVEL:** Watcom 10.0 rejects `-6` (E1073) → original uses at most `-5` (Pentium).
  Switched default `WATFLAGS` from `-6r` to **`-5r`** (2/2 matches still pass). `-5` vs
  `-4` vs `-3` still TBD, pin on a scheduling-sensitive function.
- Ghidra headless ran with the raw code segment at **base 0x0** (addr == file offset,
  convenient for byte extraction). Absolute data refs into OBJECT2/4 don't resolve at
  base 0, fine for now, revisit base if analysis quality needs it.

### Headless inventory command (reference)
```powershell
$env:JAVA_HOME="C:\Program Files\Eclipse Adoptium\jdk-21.0.11.10-hotspot"
& C:\ghidra\ghidra_12.1.2_PUBLIC\support\analyzeHeadless.bat `
  C:\Users\James\freesynd\ghidra SyndDecomp `
  -import C:\Users\James\freesynd\inputs\SYNDICAT_MAIN_OBJECT1.linear.bin `
  -processor x86:LE:32:default -loader BinaryLoader -loader-baseAddr 0x0 -overwrite `
  -scriptPath C:\Users\James\freesynd\ghidra_scripts `
  -preScript SeedDisassemble.java `
  -postScript ExportFunctions.java C:\Users\James\freesynd\manifest\functions.json
```

## Session Log

- **2026-07-10**, Project defined (matching decomp, function-level + manifest).
  Confirmed Watcom/LE/DOS-4GW. Chose hybrid hosting (container core in WSL2 Docker
  + native host RE tools). Built `synd-decomp` image, proved compile+disassemble
  loop. Wrote scaffold + this memory file.
- **2026-07-10 (cont.)**, Installed JDK 21 + Ghidra 12.1.2. Ran headless inventory:
  357 functions → `manifest/functions.json`. Analysed leaf candidates, discovered
  frame-pointer omission (34/357 framed) and that pure leaves are asm/runtime
  primitives with custom register conventions. Refined first-match strategy toward a
  small standard-convention non-leaf function + objdiff relocation handling. Deferred
  GhidraMCP (needs restart) to the matching phase. Next: build `tools/match.py`.
- **2026-07-10 (cont. 2)**, Built `tools/match.py` (compile+byte-diff loop) and a
  LOCAL live cockpit (`tools/dashboard.py` → `dashboard/`, served on :8777, user does
  NOT want a claude.ai Artifact). User questioned the 357 count, correct: it was a
  lower bound (only ~27% of the code segment covered). Aggressive analysis + entry-point
  seed raised it to 500 (~40%). Root cause of the gap: the `.linear.bin` has **no LE
  fixups applied**, so jump tables / function pointers are unresolved. Parsed the LE
  header, **object 1 (code) base = 0x10000, entry EIP offset = 0x2d85c**, objects at
  0x50000/0x70000/0x80000. Installed the **yetmorecode LX loader** (v12.0.1, patched to
  12.1.2), it maps object1 at the right base with fixups, BUT crashes reading object 4
  (`readBytes` past EOF) on both the extracted `.LE` and the `.EXE`. So the complete,
  relocation-correct inventory is still TODO. Current working manifest: 500 fns at
  base 0. **Reorganised the repo:** decomp promoted to root (own fresh git repo),
  FreeSynd/VibeSynd port moved to `legacy/` (git-ignored, own `.git` kept). Fixed
  cockpit charset (was missing `<meta charset>` → mojibake). Next: LX-loader object-4
  workaround (or hand-roll fixups) for a full inventory, then first real match.
- **2026-07-10 (cont. 3)**, GhidraMCP brought fully online (GhidraMCP registers under
  the **Utility** plugin category, not Miscellaneous). Bridge connected over TCP
  (127.0.0.1:8089), 205 tools. **FIRST MATCH LANDED: `FUN_0003c46d`, 100% byte-identical**
, the whole Ghidra→C→Watcom→wdis→byte-diff loop proven on a real function. Found +
  fixed the manifest `image_base` bug (was 0, real base 0x10000) that made match.py read
  wrong target bytes. Coverage 1/500. Next: knock out the sibling accessors, then
  non-leaf game logic.
- **2026-07-10 (cont. 4)**, Matched sibling accessor `FUN_0003c473` (**2/500**). Deep-dived
  a framed `return 0` (`FUN_0003ca0d`): OW v2 can't emit its lean epilogue. Hypothesised
  we needed period Watcom → **downloaded + stood up Watcom 10.0a under DOSBox in the
  container** (`tools/wcc_dos.sh`, tree in `toolchain/`). Result: **10.0a == OW v2** on all
  tests, so that hypothesis was wrong, the lean epilogue is an open puzzle (maybe 9.5 /
  a flag / hand-asm), parked. Real gains: the period toolchain is now available, and
  learned 10.0 has no `-6` CPU level → switched base flags `-6r`→`-5r` (matches still
  pass). Built `tools/le_fixups.py` earlier this session too (relocation resolver).
- **2026-07-10 (cont. 5)**, Game uses **stack calling** (`-5s`) for many fns (params at
  `[ebp+8]`/`[esp+4]`). Disproved CLIB hypothesis (game's strcpy/toupper are custom, not
  Watcom lib). **Big result: OW v2's register allocator DIVERGES from period Watcom**, on
  `FUN_00018878`, OW v2 picks EAX, 10.0a picks EDX (matching original). **⇒ Watcom 10.0a
  (`wcc_dos.sh`) is the correct compiler, not OW v2.** Built `tools/match_reloc.py`
  (relocation-aware diff, masks call targets), **matched FUN_00018878 → 3/500** (10.0a,
  `-5s`, reloc-aware). Framed lean-epilogue class still blocked on both compilers.
- **2026-07-11 (cont. 8), WATCOM 9.5 A/B TEST → 9.5 hypothesis disproven, framed class = runtime
  library.** Downloaded Watcom C/C++ **9.5b** (1993) from WinWorldPC (product `/watcom-c-c/90`,
  file `Watcom CPP 9.5b (1993).7z`, 31.6 MB, direct-curl the interstitial's `/from/<mirror>` link
  with a browser UA + referer). Floppy imgs → 7z-extract `WCC386.DOS` from `W9532_06.img` → WPACK-
  decompress (same `03 24 01 01` format as 10.0a, used 10.0a's `WPACK.EXE` under DOSBox,
  `tools/wunpack95.sh`) → real `WCC386.EXE` (627 KB, banner "WATCOM C32 … Version 9.5"). Staged at
  `toolchain/watcom95/BIN` (+ DOS4GW reused from 10.0a), wrote `tools/wcc_95.sh`, `tools/match95.sh`.
  **Result: 9.5 == 10.0a byte-for-byte on the framed-logic fns** (frameless still matches exact too),
  so the framed mismatch is NOT a version thing. **Key realization: all 34 framed fns cluster in
  `0x3a000-0x3e600` = the linked-in C runtime library**, precompiled by Watcom → don't recompile
  them from C, byte-match vs CLIB `.obj` or deprioritize. Net matches unchanged (43), this was a
  hypothesis-killing investigation that redirects effort to the frameless game code. Tooling for
  9.5 kept (working) for future A/B tests. **THEN PROVED the library claim** (`tools/libmatch.py`):
  unpacked the 9.5 `CLIB3R/S.LIB` (WPACK), byte-searched them + 10.0a's for each framed fn's code, 
  ~37 are HIGH-coverage (their bytes ARE in CLIB: tolower/toupper/abs/fgetc/etc.), the rest are
  low-coverage GAME wrappers. Corrected cont.5's mistaken "0x3dfcf is custom not CLIB" (it's 100%
  library). Framed block = library ⊎ game-wrappers, cleanly separable by libmatch coverage.
  **NAMED them** (`tools/libname.py` → `manifest/library_functions.{json,md}`): mapped each framed
  fn to its CLIB module via `wlib` module offsets. 37 library (strcpy, tolower, toupper, labs, isatty,
  outp, segread, stricmp, strnicmp, strncmp, strchr, fopen/fgetc/fclose/fread/ftell/open/lseek/tell/
  unlink, ltoa, atol, spawnve, makepath, cenvarg…), 13 probable, 15 game-wrapper. ≥90% cov = reliable
  name, 40-89% = module best-effort, <15% = game. So the "blocked framed-logic" fns are IDENTIFIED
  runtime library, skip them, decomp targets the frameless game code.
- **2026-07-11 (cont. 7), FRAMED CLASS CRACKED → 43/500 (+18).** Root-caused the long-standing
  "framed lean-epilogue blocked" note as a **flag error**: prior sweeps only tried `-of+` (adds
  `mov esp,ebp`), plain **`-of`** gives the lean epilogue. `-3s -of` (‑3 ⇒ direct `push [mem]`
  instead of ‑4/‑5's `mov reg,[mem]; push`) matches the whole **framed forwarder / call-sequence**
  class. Banked 12 framed (0x3db69, 3aee6, 3aa74, 3ab59, 3addb, 3b8f8, 3cfce, 3e48e, 3c002 indirect,
  3e471, 3cc45, 3b8cd, 3b972, 3b407). Then confirmed **frameless push-ebx matches out-of-the-box on
  10.0a** with plain `-4s`, 5 EXACT first-try (0x34168, 0x25378, 0x18488, 0x26aa8, +). Built
  `tools/framed.py`, `tools/byprefix.py`, `tools/wcc_la.sh`. **Big strategic finding:** A/B-tested
  10.0a vs 10.0 LA on the still-failing framed-LOGIC fns, both diverge IDENTICALLY from the
  original in 4 ways (register-save order, cond-test-on-mem, `leave` vs `89ec5d`, leaf frames). ⇒
  **original compiler ≈ Watcom 9.5, acquiring it is the top next step** (unlocks the framed-logic
  class wholesale). Frameless + framed-forwarders remain productive on 10.0a. Parked: switch-table
  dispatchers, variadic, asm primitives.
- **2026-07-10 (cont. 6), AUTONOMOUS MATCH LOOP → 25/500.** Baked DOSBox into the image
  (fast ~2s/attempt). **CPU-LEVEL CALIBRATION: game built with `-4` (486), NOT `-5`**, 
  `FUN_00026ba8` needs `movsx` (`-3`/`-4`), `-5` emits the Pentium `mov+sar` trick. New
  base = **`-4s`** (stack) / **`-4r`** (register), per-function by param source. Generalized
  `match_reloc.py` masking to ALL 4-byte external-reloc placeholders (calls, jmps, abs32
  data operands), unlocks getters/setters. **Matched patterns:** dispatch-stub cluster
  (13, `mov eax,imm;jmp` → `return h(imm)` tail-call), int getter (`a1`, extern global),
  const setter (`c705`), call-sequences (`a();b();c();d()` w/ tail-call), forwarders
  (1/3-arg), sum-of-squares `g(a*a+b*b)`, field-clear `g(p);p->f=0`. **Tools added:**
  `match10.sh` (compile 10.0a + reloc-diff), `batch_stubs.py`, `batch_match.py`, `mark.py`,
  `pick.py`, `triage.py`, `smallest.py`. **Gotchas:** getters need `extern` global (not
  literal cast) for the direct `a1`/`803d` abs form, some over-simple fns have a spurious
  `push ebx` we can't reproduce (e.g. FUN_00036168), runtime-computed constants (mov+sub)
  won't match a folded literal (FUN_00022708). **Blocked classes:** framed lean-epilogue
  (~35), asm primitives (INT/OUT/segment/rep), carry-flag-convention (FUN_0003e698).
