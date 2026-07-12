# The matching playbook (living doc)

Consolidated, deduplicated knowledge for byte-matching Syndicate functions with Watcom C/C++
9.5b. This is the single reference the orchestrator and every agent should read first. It is
**updated as new data arrives** — when an agent discovers a lever or a wall, it goes here.

Companion: `docs/object-model.md` (the pool/entity field map + global-table catalogue).

## 0. Method

- **The disassembly is authoritative.** The Ghidra *decompile* is a hint that regularly invents
  params, gets signedness/width wrong, mistypes `__thiscall`, and shows bogus `CONCAT`/`unaff_`
  artifacts. Always confirm against `disassemble_function`.
- **Goal = relocation-aware (masked) match**, not exact: `RELOC-AWARE match (masked): YES`. The
  only allowed differences are the masked fixup bytes (absolute addresses / call rel32s).
- **Sibling-reference first.** If an already-matched twin exists (same shape, different globals),
  read its `src/*.c` and mirror it — these match first-try in seconds. Look for cousins before
  writing from scratch.
- **Bounded effort.** Hand-iterate a dozen or so times; if it won't close, report the NEAR-MISS
  (best X/Y + the specific remaining diff) — the orchestrator runs a serial `--workers 32` fuzzer
  on near-misses. Don't exhaustively grind register walls (see §3).
- **Compile+diff:** `docker run --rm -v "$PWD":/work -w /work synd-decomp bash tools/match95.sh
  <FUN> "<flags>"`. The work dir is now isolated per-invocation, so parallel compiles are safe.
- **If "ours" bytes look wildly wrong** (phantom DIV/loops), `rm -f build/<FUN>.obj` first — a
  stale object was picked up.
- **No `\` in char/string literals.** A backslash literal (e.g. `'\\'`) makes the DOSBox-hosted
  9.5 compile silently fail ("COMPILE FAILED (no OBJ)", empty log) — the byte gets mangled in the
  DOS file copy. Use the numeric value instead (`92` for `\`, etc.). Cost me a match on 0x3e361.
- **Compiler/RTL identity is settled: Watcom C/C++ 9.5, small-model `CLIB3S` (stack-calling).**
  Proven by RTL fingerprint (`tools/libname.py`): every C-runtime fn in `0x3a000+` maps to a `95S`
  library module (16 byte-identical), zero to 10.0a. We have the exact toolchain — remaining
  register-role walls are NOT a wrong-version artifact, they're genuine 9.5 source/flag sensitivity.
- **`0x3a000+` C-runtime functions are identifiable stdlib, not game code.** `tools/libname.py`
  names each one's library module (strcpy, strncmp, atol, fread, fgetc…). The `-d2` leaf recipe
  above matches the plain-C ones; ones with `int 21h`/`out`/`in` (isatty, outp, lseek, tell) are
  hand-asm — park as `library:<module>`, don't grind them as mystery game code.
- **If match95 fails only on a length check**, the manifest `size` may be wrong (headless pass
  under-counted; e.g. 0x29c58 was 177 vs true 238). Confirm the extent with `get_function_by_address`
  and correct the manifest `size`. NOTE its `Body: START - END` end is INCLUSIVE, so
  `size = END - START + 1` (off-by-one here mis-splits the jump-table tail-compare by 1 byte —
  0x1bc28 was 1333 vs true 1334).
- **Inline jump-table switch fns — NOW VERIFIABLE.** Watcom co-locates a `switch`'s jump table
  (+ entry-alignment pad) in the SAME object `.text` BEFORE the code (`[table][pad][code]`), while the
  real binary keeps the table in a far segment (`CS:[...+disp]`) and the on-disk fn is clean code.
  `match_reloc.py` now DETECTS this (a leading run of >=4 consecutive 4-byte fixups at obj offset 0 =
  the table) and compares only the code TAIL (`ob[-size:]`) with re-based fixups, printing
  `JUMP-TABLE-AWARE match : YES`. Byte-equality is still fully enforced (a wrong split fails, never
  false-matches). So switch dispatchers are matchable like any other fn — write byte-faithful C and
  it verifies. (0x23038 banked this way.) Prereq: the manifest `size` must be the TRUE extent (these
  fns are almost always under-counted — the headless sweep truncates at the indirect `jmp CS:[..]`).

## 1. Recipes (compile flags)

| recipe | when |
|--------|------|
| `-4s -oneatx -zp8 -s -zq` | **default** — stack-calling game code (< 0x39000) |
| `-4r -oneatx -zp8 -s -zq` | `__fastcall`/register-calling fns; the `0x39xxx` region |
| `-3s -of -oneatx -zp8 -s -zq` | framed runtime-library region (`0x3a000+`, non-leaf), manual `push;mov;sub` frame |
| `-3s -d2 -oneatx -zp8 -s -zq` | **LEAF runtime-library fns** (`0x3a000+`) that have a full `55 89 e5 … 5d c3` ebp frame yet call nothing. `-of` only frames *callers*, so leaves stay frameless (`8b 44 24 04`) and never match. `-d2` (full debug) FORCES the ebp frame on leaves; `-oneatx` keeps the body optimized so only the frame is added. The CLIB3S runtime was built `-d2`-optimized. Unlocked the C-runtime leaf class: labs 0x3aed8, toupper 0x3dce5, tolower 0x3da37, strchr 0x3e7f7, strcpy-variant 0x3dfcf, path-sep helper 0x3e361, nibble→hex 0x3b9ee. |
| `-3s -os -zp8 -s -zq` | `0x3a000+` fns whose prologue is `ENTER 0x2c,0`/`LEAVE` — `-os` emits ENTER where `-of` emits the manual frame (byte-0 divergence otherwise). Add `-ol` for two-step `mov al;movzx` loads. (0x3cc26) |
| `-4s -or -zp8 -s -zq` | when -oneatx hoists a stack param into a callee-saved reg the target RE-READS each use (-or (reorder only) skips that hoist). e.g. 0x38fe8, 0x377b8 |
| `-4s -ot -s -zq` (or `-oe/-or/-os`) | the occasional less-optimised unit (target bigger/un-folded than -oneatx) |

Determine the convention from the disasm: params from `[ESP+..]` → `-4s`; params from ECX/EDX →
`-4r`. When unsure, try both.

## 2. Levers (steer Watcom from C — these CLOSE near-misses)

- **C-runtime functions: use Open Watcom's own source.** The `0x3a000+` region is CLIB3S
  (Watcom 9.5, proven by RTL fingerprint). For any function `tools/libname.py` names (strcpy,
  strncmp, atol, fread…), fetch the real source from GitHub `open-watcom/open-watcom-v2` under
  `bld/clib/` (strings in `bld/clib/string/c/`, e.g. `strncmp.c`, `stricmp.c`, `strcpy.c`) and
  translate it (headerless; `CHAR_TYPE`→`char`, `UCHAR_TYPE`→`unsigned char`, `NULLCHAR`→`0`,
  `STRING('A')`→`'A'`). OW v2 differs subtly from 9.5, so expect to adjust (e.g. 9.5 increments
  loop pointers at the BOTTOM, `++s;++t;`, where OW v2 uses inline `*s++`). Match with the leaf
  recipe `-3s -d2 -oneatx`.
- **HAND-ASM CLIB functions: replicate the `#pragma aux` verbatim.** Some CLIB funcs are inline
  asm, not C (strcpy's 386-small path is `#pragma aux __strcpy`, a 2-byte-unrolled copy). A plain
  C loop can NEVER match these (that's what walled 0x3a8d7). Instead lift the exact pragma from OW
  `bld/clib/string/c/*.c` and call it. WORKS on 9.5 incl. `L1:`/`je short L2` labels. Put the whole
  pragma on ONE physical line (no `\` continuations — DOSBox mangles backslashes). If the mini-asm
  picks a shorter encoding than the target (e.g. `add eax,2` → `83 c0 02` but target has the
  EAX-accumulator `05 02000000`), force the exact bytes with `db` directives (`"db 5" "db 2" "db 0"
  "db 0" "db 0"`). This banked strcpy 0x3a8d7 byte-exact and generalizes to outp/memcpy/etc.
- **DOS/port asm fns: replicate as a `#pragma aux` wrapper, db-transcribe the body.** For the
  small hand-asm RTL fns (`out dx,al`, `int 21h`, `mov Sreg`), write `extern T __f(...); #pragma aux
  __f = <body> parm[..] value[..] modify exact[..]; T FUN_x(..){ return __f(..); }` and let `-d2`
  add the frame + param loads. Emit the body as `db` bytes (the mini-asm rejects some mnemonics,
  e.g. `mov eax,cs`; db is universal). Pass byte args in the 8-bit reg (`parm [al]` → `mov al,[..]`
  not `movzx`). Reloc'd globals (e.g. the DOS-version flag `ds:0xc2da`) can be emitted as LITERAL db
  bytes — they equal the resolved address in the linear.bin, and match_reloc only masks OUR obj's
  fixups (none here), so a literal compares equal. Banked: outp 0x3b22d, segread 0x3b3b9, isatty
  0x3c44d, d_getvec 0x3b239. When the wrapper's param loads don't match (e.g. d_setvec loads its
  segment arg 16-bit as `mov cx`, but a 32-bit stack slot always gives `mov ecx`), emit the WHOLE
  body INCLUDING the `[ebp+N]` param loads as db bytes with `parm []` — `-d2` supplies the ebp frame
  and the params sit at `[ebp+8..]`, so the db `mov cx,[ebp+0x10]` matches exactly. Banked d_setvec
  0x3b273 this way.
- **call-in-pragma: mix `db` bytes with a real `call <extern>`.** For DOS-asm fns that call an
  error handler etc., emit the body as `db` bytes but replace the `e8 xx xx xx xx` with a real
  `"call FUN_<callee-abs-addr>"` string (declare it `extern`). The mini-asm emits a rel32 fixup to
  the symbol, match_reloc masks it → RELOC-AWARE YES (EXACT will say NO on the masked call bytes,
  which is fine). Compute the callee's absolute addr from `func_addr + call_off + 5 + rel32`. This
  banked lseek 0x3a93b, tell 0x3a97c, open 0x3a579, qread 0x3d935 (all call the same DOS-error
  handler 0x3c4b9), and the switch-char helper 0x3cbf9 (int21, no call). LIMIT: frameless asm stubs
  (no `-d2` frame to hang the body on — 0x3a37a bswap, 0x3cabb stack-check, 0x3b1f6 _exit) and real
  C fns with buffer logic (ftell, chktty, fgetc, fread, ioalloc — write real source, don't
  db-transcribe compiled C; they carry register-role risk like the string fns) stay parked.

- **Branch layout** — invert the `if`/`else` so the target's *fall-through* path comes first
  (matches its `JZ`/`JNZ` sense). For multi-way dispatch, use explicit `goto`s to mirror the exact
  CFG block order, and write shared tails once so Watcom tail-merges them.
- **Return width** — `unsigned short` return → `xor ah,ah`; `int` → `and eax,0xff`; `char` → bare
  AL (no widen). Pick the return type from the epilogue.
- **Two-step zero-extend** — cast a call arg `(unsigned short)x` to get `movzx di; and edi,0xffff`
  (byte→ushort→uint), vs one-step `movzx edi` for a plain `(unsigned)`.
- **Sign vs zero extend of a memory operand** — the DEREF-CAST type decides: `*(unsigned short*)p`
  → `movzx` (zero-extend); `*(short*)p` → `movsx`. A later `/2` still emits signed `sar`+correction
  because the value promotes to `int`.
- **Immediate form** — write `>= 0x19` not `> 0x18` to emit the target's exact `CMP …,0x19`.
- **Unsigned compares** — `JBE`/`JAE`/`JC`/`JNC` in the disasm ⇒ the operands are `unsigned`
  (`unsigned short`), not signed. Signed would be `JLE`/`JGE`.
- **Operand orientation** — write the compare so the operand the target loads FIRST is on the side
  that gets the register (e.g. point-first `g_pt > g_box` when the target does `mov ax,g_pt; cmp
  ax,g_box`). Also controls which of two commutative operands lands in which reg (e.g. `x*0x10 + y`
  vs `y + x*0x10` decides whether `x*16` is EDX or EBX).
- **Multiply grouping** — `a*(b*c)` vs `(a*b)*c` fixes the `IMUL` order.
- **Symbols not literals** — use `extern` symbols for pointers/bounds so `p < end` can't
  constant-fold (the target keeps its top guard). A size arg compiled as `mov eax,END; sub
  eax,START` is `g_END - g_START` (two distinct externs → runtime subtraction, not a folded const).
- **Loop form** — `if (start < end) do { … } while (p < end);` → top-guard + do-while (loop
  rotation, what -oneatx does for a `while`). A **goto-rotated** counted loop `goto test; do { … }
  test:; while (i < n);` reproduces `xor ebx,ebx; jmp cond; body; cond; jc`. Plain `while` often
  compiles to jump-to-test instead.
- **Declaration / statement order** — the order locals are declared/first-used sets both register
  assignment and the initial load order (e.g. `n, dst, src` with `n` unsigned reproduced count→EBX,
  dst→EDX, src→EAX). The permuter's statement-reorder automates this.
- **Param reassignment** — reassign a param in place (`param_3 = f(param_3);`) so it reuses its
  stack slot instead of Watcom allocating a fresh local (avoids a `sub esp`).
- **Load/store split** — a named temp `t = obj[4]; …; obj[3] = t;` separates a field's load from its
  store when the target schedules them apart.
- **Drop phantom params** — if a param's stack offset is too high, the decompiler invented leading
  params; use the real arg count (fixes all downstream offsets). Same for a mistyped `__thiscall`.
- **Callee arg width** — declare the callee's params at the right width; a `short`/`unsigned short`
  param makes the caller push a sign-/zero-extended 16-bit value.
- **`volatile`** — mark a global `volatile` if the target RE-READS it each use where -oneatx would
  CSE it into one register (e.g. mouse/interrupt-updated coords).
- **AND-immediate width** — `x &= ~0x208` emits the sign-extended `and edi,0xfffffdf7`; `x &= 0xfdf7`
  emits `0x0000fdf7`. Match the target's immediate by choosing `& ~mask` vs `& mask`. (0x30708)
- **Reuse a CMP for a cluster** — write the 2nd test as `c <= 2` (not `c < 3`) so Watcom reuses the
  prior `cmp si,2` as `jbe` instead of a fresh compare. (0x30708)
- **Signed vs unsigned `char`** — Watcom `char` is UNSIGNED (a `char` compare emits `JB`/`JAE`).
  Use `(signed char)` to get the target's signed `JL`/`JGE`. (0x22ba8)
- **`#pragma aux <FnName> modify [eax ecx edx ebx];`** — puts a callee-saved reg in the volatile
  set, suppressing a spurious `push/pop ebx` when the target uses EBX as unpreserved scratch. (0x180f8)
- **`#pragma aux <FnName> aborts;`** — declare a noreturn callee (DOS exit / abort) so Watcom
  TAIL-JUMPS it (`jmp`) without restoring saved regs, matching the target. (0x18338)
- **Duplicate vs goto-share a tail** — sometimes DUPLICATE the shared tail into BOTH branches
  (not a `goto`): Watcom cross-jumps/tail-merges it AND defers the cdecl `add esp,N` to ONE merge
  point, whereas `goto`-sharing forces eager per-branch pops. (0x36c78; opposite of the usual advice)
- **Materialise a node pointer** — write `n = g_810e + id; n[0x1e]=…` (not inline `g_810e+id+0x1e`)
  to force `add ebx,0x810e; [ebx+0x1e]` instead of folding the base into the store disp. (0x37878)
- **Nested-call arg overlap** — when an inner call's result feeds an outer call, Watcom OVERLAPS
  their pushed stack args (shares the trailing ones) and cleans up with one `add esp,N` sized for the
  union. Reproduce by giving the inner callee the extra trailing arg(s) it appears to "share" so the
  cleanup width matches (e.g. inner `FUN_x(...,0xc)` makes the merged pop `add esp,0x14`). (0x361a8)
- **Inline vs named temp** — pasting a subexpression at each use forces Watcom to CSE it into a
  persistent (callee-saved) register with `xor eax,eax; mov ax,bx`; a named local instead keeps it
  in a scratch reg zero-extended in place (`and eax,0xffff`). Also drop redundant `(int)` casts.

## 3. Walls (recognize, then PARK — not source-reachable)

If the structure is byte-correct and only ONE of these remains, stop and park with a note. Do not
grind; the fuzzer permutes *source* and can't change the allocator's mind.

- **Register-role tie-break** — identical instructions, but a value lives in a different register
  (EAX vs EDX / ESI vs EBX), cascading into encodings (`cwde`↔`movsx`, `lea`↔`add`, `test al`↔`test
  dl`, `cmp ax`↔`cmp dx`). Every C spelling + the fuzzer converge to the wrong reg. (0x34048, 0x34088
  CMP-modrm, 0x2dd48, 0x36d18, 0x183e8 via LICM.)
- **CSE / loop-invariant-hoist wall** — -oneatx re-reads-once (CSE) a value the target reads twice,
  or hoists a loop-invariant global load into a callee-saved reg the target reloads each iteration;
  lighter recipes change too much else. (0x26da8, 0x269d8, 0x20568, 0x1b858, 0x1b798.)
- **Loop align-vs-unroll** — the loop-alignment NOP padding only comes bundled with `-ol`, which
  also unrolls the counted loop; no flag gives align without unroll. (0x35ed8.)
- **Cross-function tail-merge** — the fn's `return 0` jumps into an *adjacent* function's shared
  `xor eax,eax; ret` stub; unreachable when compiling one fn in isolation. (0x37818.)
- **Intra-function tail-merge** — target shares ONE small `pop;pop;ret` (or `mov eax,-1; jmp END`)
  epilogue across two exits (`eb<disp>` jmp to it); Watcom 9.5b DUPLICATES the 2-3 byte tail instead
  of cross-jumping. Single-instruction near-miss; early-return / single-exit `int r` / explicit
  `goto`-to-shared-return source forms and `-os`/`-ol`/`-oa`/`-ot`/`-oc`/`-oi` all fail to induce the
  merge. Block-layout fuzzer might close it. (0x3d3e4 41/43, 0x3b99e 42/44.)
- **Intrinsic won't inline** — target uses `rep movs`/`repne scasb` (inlined memcpy/strlen); our
  batch compile has no `<string.h>` on the include path and neither `-oi` nor `#pragma intrinsic`
  triggers it. (0x299c8, 0x17998.) NOTE: `_fmemset` DOES inline via `#pragma intrinsic(_fmemset)` + a far decl (0x28728) -- the wall is `strlen`/`memcpy`-specific, not universal.
- **Scheduling / param-load order** — high-param-pressure fns where the exact instruction schedule
  (which param loads first) isn't source-reachable. (0x15e38.)
- **No 64-bit integer type (compiler limit)** — Watcom 9.5b rejects `unsigned long long` (E1060) and
  `__int64` (E1009). A target that does a true 64-bit muldiv (`mul ecx; div ebx` carrying EDX from
  mul straight into div, no `xor edx`) is UNREACHABLE — with `unsigned int` Watcom always emits
  `xor edx` (or strength-reduces the `*const`) before the unsigned `div`. (0x39495.)
- **Push imm8-vs-imm32 peephole threshold** — target encodes a small `push <const>` as imm32
  (`68 70 00 00 00`) where our Watcom 9.5b always emits the sign-extended imm8 (`6a 70`). Body can be
  byte-faithful with all relocs aligned and this single 3-byte push be the only diff. Not
  source-reachable (8 source forms × 7 recipes all give `6a`); a micro-version peephole difference.
  (0x16678 187/190.)
- **Cluster-wide dead callee-save** — some 0x39xxx fns wrap a frameless body in a lone dead
  `push ebx`/`pop ebx` (EBX reserved across a call with nothing live) — a region-wide pessimistic
  save. `-oneatx` saves no reg; `-od` saves ebx+esi+edi AND adds an ebp frame; never exactly one
  frameless EBX save. Also toolchain tells `8b ec` vs our `89 e5` (mov ebp,esp), `mov eax,0` vs
  `xor eax,eax` — the 0x39xxx region shares the RTL different-Watcom flavor for some fns. (0x39188.)
- **Prologue reg-save-order (framed CLIB region 0x3a000+)** — a framed fn that ALSO saves a callee
  reg has prologue `push ebx; push ebp; mov ebp,esp` (saved regs BEFORE the EBP frame; args at
  `[EBP+0xc+]`). Watcom 9.5b at every optimized recipe (`-3s -of -oneatx` and all `-o*` variants,
  `-3`/`-4`) emits frame-FIRST `push ebp; mov ebp,esp; push ebx`; only `-od` gives regs-first but
  de-optimizes the body. Diverges at byte 0. Recognize: the target has `push <reg>` before
  `push ebp`. PLAIN framed siblings (no saved reg, bare `push ebp; mov ebp,esp`) DO match with
  `-3s -of`. (0x3db36, 0x3dbeb, 0x3d3e4, 0x3ca0d — bodies are byte-faithful but walled.)
- **0x3a000+ framed runtime-library = toolchain-version mismatch (whole-region wall)** — the shipped
  RTL objects were built with a DIFFERENT Watcom than our 9.5b. Two irreducible tells: (a) it
  materialises the first call arg into EAX before pushing (`mov eax,[ebp+8]; push eax`) where our
  9.5b pushes the memory operand (`push [ebp+8]`); (b) the prologue reg-save-order wall above. Both
  are byte-0 / near-miss divergences no `-o*` recipe or `register` temp fixes. CONSEQUENCE: in the
  0x3a000+ region the reliably-matchable shapes are bare **plain-frame leaves** (single `push ebp;
  mov ebp,esp`, no saved reg, no guarded call materialising an arg) AND **ENTER-frame fns via `-3s
  -os`** (0x3cc26); skip guarded-wrapper / saved-reg-before-frame fns (those hit the prologue wall).
  (0x3c491 21/23, 0x3c479 21/24, 0x3c42d 28/32 all walled.) The 0x39xxx game region (`-4s`/`-4r`,
  our own compiler) does NOT have this problem — mine there instead.

## 4. Object model (see docs/object-model.md)

Pool A entity record (92B, `node = g_810e + id`, links are 16-bit ids not pointers): coords s16 at
`+4/+6/+8`, flags `+0xa/+0xb`, type/frame `+0x18/+0x19`, facing `+0x1a`, links `+0/+2/+0x1c/+0x24`,
health `+0x14`(word)/`+0x54`(byte). Direction tables `g_ab60/g_ad60` (s16[256]); grid `g_10e`
(u16[128*128]); screen buffers `g_5368/g_5370`. `(short)ptr + 0x7ef2` and `ptr - g_810e` are the
ptr↔id idioms.

---
*Changelog: created cont.14 from the parallel-agent run. Append new levers/walls here as found.*
