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
- **cpermute is jump-table-aware AND declaration-permuting (cont. 24).** The fuzzer now (a) splits
  the co-located jump table off obj `.text` before scoring (so dispatcher near-misses score
  correctly), and (b) permutes the leading DECLARATION BLOCK — exhaustively for ≤7 top-level
  locals, sampled for ≥8 — as a dedicated always-compilable phase. USE IT to settle whether a
  near-miss is decl-order-reachable: if a spill-slot/register-role residue survives the pure decl
  permutations, it is a GENUINE allocator wall (§3), not unfound C — stop hand-grinding decl order.
  SCOPE: decl-perm moves slots for STRAIGHT-LINE slot-homed locals (matched 0x179f8 via a decl
  swap; steered 0x18ae8 endpoints); it does NOT move LOOP-CARRIED slots (0x2e5f8 y↔i, confirmed
  inert) or pure register-role ties (0x34048 cur/d EAX↔EDX, confirmed inert). LIMITATION: only
  top-level decls; block-scoped locals (switch-case counters, e.g. 0x338d8) are not yet reached —
  extending decl-perm to nested compounds is the next tooling step.
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
- **`__interrupt __far` handler ⇒ `push cs`** — an interrupt-vector install whose target bytes are
  `0e 68 <ofs>` (push cs; push offset) is passing a near function as a FAR pointer. Declare the
  handler `extern void __interrupt __far H(void);` and the installee param `void (__interrupt
  __far *)(void)`; a plain cast route materialises CS via EAX instead. A 6-byte far-ptr GLOBAL
  (`void (__far *g)(void)` at ofs/ofs+4) assigned from a DX:EAX-returning callee stores DX first,
  then EAX. (0x254a8)
- **memcpy intrinsic** — prototype memcpy yourself + `#pragma intrinsic(memcpy)` reproduces the
  inline `mov eax,ecx; shr ecx,2; F2 rep movsd; mov cl,al; and cl,3; F2 rep movsb` block (note
  Watcom's REPNE F2 prefix, not F3) wrapped in push edi/pop edi. (0x17998)
- **volatile split (read vs write)** — `volatile` on an extern byte global does TWO things: the
  read becomes a direct `cmp byte [mem],imm` (instead of an eager load into a reg), AND its
  post-call STORE schedules cleanup-first (`add esp,N` before the `xor reg,reg`). If the target
  mixes idioms per site (cmp-mem read but xor-first store), declare a volatile extern for the
  reads and a SECOND non-volatile extern alias name for the writes — alias symbols are free since
  the differ masks fixups. (0x35638)
- **Two stores tail-merge, widen duplicated** — target shape `widen; mov dl,1; …; jmp M / widen;
  xor dl,dl; M: mov [reg+disp],dl` = TWO literal stores (`arr[i]=1;` / `arr[i]=0;`) whose final
  store instruction tail-merged — do NOT write a shared `arr[i]=v` (that emits ONE widen). The
  merge only fires if both arms pick the same value reg: inline the sibling byte expression
  (see inline-vs-named) so it CSEs into CH and frees DL for both arms. (0x279f8)
- **Based-pointer `:>` for far memory** — Watcom `sel :> (uchar *)off` = MK_FP. Do arithmetic on
  the FAR pointer (`p + 6`, `fp + 0x40`), not on the offset before construction, to fold into the
  displacement (`gs:[eax+6]` vs `lea`/`add`). A far-ptr LOCAL whose segment register gets
  re-pointed mid-sequence is loaded SPLIT (`mov bx,[g+4]; mov eax,[g]`) with the selector
  shadowed in BX; read the selector as a value via `*((unsigned short *)&g + 2)` and it CSEs onto
  that BX (widen `xor edx,edx; mov dx,bx`). Do NOT use i86.h's FP_SEG pragma (parm [eax dx]
  re-homes the selector to DX). A fresh far deref after a call re-materialises with `lgs`. (0x28628)
- **Nested call inside an outer arg** — `outer(f(inner(x)) …, y)` makes Watcom push `y` EARLY
  (before evaluating arg1's inner call), then split the cdecl cleanups oddly (`add esp,4` after
  the inner call, `add esp,8` after the outer). If target cleanups look misaligned with your call
  boundaries, an arg you attributed to the inner call may belong to the outer one. (0x30508)
- **`&= ~C` on a signed short lvalue** — compound `*(short *)(p+0xa) &= ~0x208;` keeps the value
  in ESI and emits the SIGN-EXTENDED 32-bit mask (`and esi,0xfffffdf7`); an unsigned type or a
  named temp folds the mask to `0x0000fdf7` and re-homes to EAX. (0x30508)
- **Inline vs named, part 3: in-place op vs copy-temp (and tail-merge suppression)** — a NAMED
  local is operated on IN PLACE (`inc edx` on its home reg) and identical branch tails then
  tail-merge into one copy; an INLINE re-read of the same memory CSEs onto the loaded reg but
  each use site gets a FRESH copy-temp (`mov eax,edx; inc eax`, next site edi, next esi) so
  identical-looking case bodies do NOT merge. When the target shows per-branch copies in
  rotating registers where you'd write one shared statement, inline the expression at every
  site (`*(short*)(p+0x14) = *(short*)(p+0x14) + 1;` — the loads still CSE, only the temps
  split). Same principle as the widen-form wall: named local ⇒ in-place/xor-first codegen,
  CSE temp ⇒ copy/and-form codegen. (0x2d228; cf. 0x279f8, 0x28628)

- **Invariant-mask placement: entry statement vs inline-LICM (cont. 21)** — a loop-invariant mask
  written as an ENTRY statement (`y &= 0x7f00;`) schedules the AND with the entry param loads and
  perturbs the whole entry-block load order; written INLINE in the loop body as
  `(short)(y & 0x7f00) >> 1`, LICM hoists it into the loop PREHEADER (AFTER the for-init/xor of
  the counter — target shows `xor esi,esi; …; and edi,0x7f00` in that order) and the hoisted temp
  stays SHORT-typed (per-iteration `movsx edx,di` re-materialisation). This + an OR operand swap
  matched 0x33c38/0x33cf8 (grid-hit twins). When entry load order around a loop is off, move
  invariant subexpressions between entry-statement and inline-in-loop forms.
- **Single-exit + goto for save/restore-at-every-return fns (cont. 21)** — when the target saves a
  global at entry (`mov si,[g]`) and re-stores it in EVERY return tail (duplicated
  `mov [g],si; pops; ret` blocks), the source is SINGLE-EXIT: `char ret; … ret = X; goto done; …
  done: g = save; return ret;`. Watcom itself tail-duplicates the store+ret into near branches and
  far-jumps the distant ones. Writing multi-return C with per-return restores SPLITS the saved
  local across TWO callee-saved regs (ESI home + DI store-copy with `mov edi,esi` re-copies) and
  adds a push. (0x264a8)
- **Full-width temp forces full xor widen (cont. 21, twice-proven)** — target shape
  `xor eax,eax; …; mov al,[mem]; cmp ax,dx` (32-bit clear feeding a 16-bit compare of a byte vs a
  ushort global): `(unsigned short)<byte-expr> == g` emits the half-clear `xor ah,ah`/`xor dh,dh`
  when another register already holds a zero-extended byte (16-bit temp reuses known state);
  DROPPING the cast promotes the ushort side to a 32-bit zext copy instead (worse: register split).
  The fix is a named FULL-WIDTH temp + cast at the compare: `unsigned int t = <byte-expr>;
  if ((unsigned short)t == g)` — defining t clobbers the whole register, forbidding the half-reg
  trick, and the compare stays 16-bit. (0x164c8 first, then 0x265d8 → both MATCHED with it.)
- **Register-resident param copy (cont. 21)** — when ONE param lives in a callee-saved reg from
  entry (`mov esi,[esp+8]` before any call, default-assign writes SI not the slot) while sibling
  params stay memory-homed (slot cmp/store, movzx at push), that param is used via a NAMED LOCAL
  COPY (`unsigned short bb = b;` first statement) in the original. Which param OURS auto-promotes
  instead is allocator-internal (see the promotion wall below). (0x35d08)
- **2D-table address association is spelling-controlled (cont. 21, menu twins)** — for
  `table[d*3 + a]` (dword table, both indices variable): `*(char **)((char *)tbl + a*4 + d*12)`
  (column addend FIRST) yields `lea eax,[edx*4]; sub eax,edx; lea edx,[eax*4]` (12d materialised)
  with the column in the modrm scale + moffs `mov al` column load; `tbl[d][a]` / row-addend-first
  yields the opposite association (4a materialised, row 3d in the modrm scale). Both twins needed
  one of EACH form at their two call sites. (0x205f8/0x20728)
- **Volatile-deref split for post-store re-reads (cont. 21)** — when the target re-reads `*node`
  from memory after a `*q = *node` store but -oneatx CSE-merges the reads, cast ONLY the post-store
  re-reads through `*(volatile unsigned short *)node` — exactly those loads split; nothing else
  changes. Pointer-deref variant of the volatile-read lever. (0x26c78; likely un-parks 0x26da8's
  CSE component.)
- **`short argc`/16-bit params stay memory-homed (cont. 25)** — an `int` param that the target
  compares 16-bit (`CMP DX,word[esp+N]`) gets promoted into a callee-saved reg (extra `push ebp`)
  as `int`; declaring it `short` keeps it memory-homed and kills the promotion — often the single
  biggest fix in a call-heavy fn. (0x24be8, the arg loop.)
- **Near strcpy/strcat inline too (cont. 25)** — `#pragma intrinsic(strcpy, strcat)` with local
  near decls DOES inline byte-exact (repne-scasb strlen + 2-byte-unroll copy), joining `_fmemset`/
  `_fstrcpy`/`_fstrcat`. The intrinsic wall (§3) is strlen/memcpy-SPECIFIC, not all string.h.
  (0x24be8.)
- **Register pressure can make a BIG fn match where its small sibling walls (cont. 24)** — a
  larger function with more live values forces Watcom's allocator into the target's exact
  register choices, where a small sibling with slack allocates freely and diverges. Counter to
  intuition, don't skip a megafn because its small cousin is a register-role wall — the extra
  structure may CLOSE it. (0x184b8 803B MATCHED where sibling 0x183e8 is walled; both DPMI
  alloc/free with the same idioms.)
- **Cross-jump law for per-case call tails (cont. 22, proven by a 6-test battery)** — -oneatx
  cross-jumps identical per-case call tails ONLY when EVERY arm exits the switch via break/
  fall-out; a single goto/continue/return arm kills merging for ALL arms. To reproduce a merged
  call block that N arms jump into: end N-1 arms with `goto hit;` and ONE arm (the one whose copy
  the target keeps, usually the last case body before the merge point) with `break` falling into
  `hit:`. This explains and unlocks the merged-call-tail dispatcher class. (0x29ad8 — MATCHED.)
- **Volatile-alias extern pins entry-load SCHEDULING too (cont. 22)** — beyond re-read
  splitting: a non-volatile `end = g_10ae0` init gets sunk 3 instructions by the scheduler; the
  volatile-alias spelling holds the `mov ebp,[g]` load FIRST. Closed 0x2d9e8 (853B). Same report:
  compound `ca *= w+1` gives in-place and+imul (assignment form splits an xor-temp); inline
  flag-byte reads home per-chain in DH/DL/AL (named uchar forces AL, +1B each); a named temp for
  a double-read can REGRESS (reuse-copy vs the target's second load) — try plain inline first.
- **Volatile-alias extern (cont. 21)** — declare a SECOND extern name for the same global with
  `volatile` (e.g. `g_e116v`) and use it per-site: volatile codegen exactly where wanted (store
  forms, mem-cmp, `=1` immediates), normal codegen elsewhere — fixup masking ignores names.
  Confirmed: wcc386 9.5 IGNORES volatile applied via cast; only the declaration counts.
  (0x2bee8 case 3; decl-volatile on the real name regressed case 1.)
- **`& ~7` vs `& 0xf8` mask spelling (cont. 21)** — with all mask high-bits set (`x & ~7`),
  Watcom's demanded-bits narrows the AND to the byte form `and al,0xf8` and copies the full
  register (quotient bits 8-15 pass through); a `(uchar)x & 0xf8` spelling inserts a movzx and
  `0xf8`/`0xfff8` int masks stay wide (`25 …` imm32). The byte-AND + full-copy signature means
  `& ~CONST`. (0x2ee18 — MATCHED; also: inline `di + 8` in the guard with ushort di makes LICM
  re-materialise per-iteration instead of spilling a hoist slot.)
- **goto-fail merge into a guard body (cont. 21)** — when the target shares ONE `xor eax; jmp`
  return-0 stub jumped into from multiple guards, write `goto fail;` with the `fail:` label
  placed INSIDE another guard's if-body — reproduces the shared stub instead of duplicated
  epilogues. (0x2e408, closed the whole back half.)
- **Addend position is value-numbering-significant (cont. 21)** — in `base + (product)` vs
  `(product) + base` arg expressions, the ADDEND's position flips a 2-cycle register allocation
  (ESI↔ECX) even where commuting the multiply itself is byte-inert. Try addend-first on ONE
  operand at a time — the same swap on a sibling arg can degrade a `cwde` to `movsx`. (0x2e408)
- **Dead `test reg,reg` before an unconditional set (cont. 21)** — the target shape
  `test ebp,ebp; mov bl,1` with no branch between comes from `r = out ? 1 : 1;` (or
  `if (out) r = 1; else r = 1;`): Watcom cross-jumps the two identical arms, deletes the
  jcc-to-next, and the test survives. (0x363d8)
- **ushort locals: demanded-bits narrowing + promotion derank (cont. 21)** — `unsigned short`
  locals activate dword slot ops with dirty upper16, half-clear widens (`mov al,X; xor ah,ah`),
  and the `xor ah,dh` known-equal-clear trick; AND they are DERANKED from callee-saved promotion
  (int-typed locals steal ESI/EDI; ushort ones stay in slots). ushort vs int-with-casts params
  also flip the evaluated load order of `a + b` slot reads, and a callee DECLARED with ushort
  params reproduces the `mov ax,dx; and eax,0xffff` truncating arg push. Use ushort typing to
  steer which values get registers. (0x363d8, 0x26778 — `int two = 2;` named constant homing in
  EBP was load-bearing in the latter.)
- **Inline far-pointer construction per use site (cont. 21, MATCHED 0x28558)** — when the target
  re-arms GS from the PARAM SLOT at each far access (`mov gs,[esp+N]` before each deref) and
  never spills a selector: construct the far pointer INLINE at every use —
  `(sel :> (unsigned char *)off)[k]`, and pass `sel :> (unsigned char *)off` directly in call-arg
  position (the seg push widens from the just-armed GS). A NAMED far local (`p = sel :> off`)
  spills its selector half to a frame slot and rotates other registers (extra push). Complements
  (does not contradict) the GS-home rules below — pick by what the target does: slot re-arms ⇒
  inline constructions; `mov dx,gs` value reads ⇒ one named far pointer.
- **`__segment`-typed selector locals (cont. 21)** — a local declared `__segment` (e.g.
  `__segment sel = *(__segment *)(out + 3);`) tolerates MULTIPLE `:>` constructions without
  re-homing the selector to a GPR (a plain ushort tolerates only one), and a fresh inline
  `sel :> (unsigned char *)0` arg produces the fresh `xor edi,edi` zero-offset. Side effect:
  `__segment` locals change stack-layout ordering (re-steer arrays by decl order after). Still
  unsolved: forcing the direct memory→sreg load (`mov gs,[esp+N]`) when the value ALSO has word
  uses — ours homes it in EDX + `mov gs,dx`. (0x27f08, improved 177→183/185.)
- **Far-pointer selector GS-home rules (cont. 21, 0x27f08)** — a selector local keeps its
  segment-register home (direct `mov gs,[mem]` load, value reads via `mov dx,gs`, word spills)
  ONLY while it feeds exactly ONE `:>` far-pointer construction; a second explicit `sel :> …`
  re-homes it to a general register. Route every deref/copy through the one far pointer.
  Anti-patterns: `(__segment)p` casts and far `p != 0` compares force a memory home; an inline
  rvalue `(ushort)expr :> ptr` can silently fall back to a DS-based pointer (WRONG CODE). A
  DS-based far global (`extern uchar __far *g;` assigned from a near pointer) emits the target's
  `mov [g+4],ds; mov [g],eax` pair and `lgs` re-materialisation at each use. (0x35d08 confirms.)

- **Anonymous global deref => and-form widen (cont. 21 retry; UN-PARKED the 0x28628 widen wall)** —
  the and-form byte widen (`mov al,dl; and eax,0xff`) fires ONLY when the pushed value is an
  anonymous compiler-owned CSE temp; ANY named local (even block-scoped, even a full-width
  `unsigned int t = st;` + `(unsigned char)t`, which gives xor+mov+AND all three) yields xor-form.
  For a busy-wait status byte, write EVERY read directly through the far-ptr GLOBAL
  (`while (g_fp[0x31] == 0xff); if (g_fp[0x31] != 0 && …) f(…, g_fp[0x31]);`, non-volatile): the
  lgs hoists, the byte load stays in-loop in DL, post-loop uses CSE onto DL anonymously =>
  and-form. A LOCAL POINTER COPY `q = g_fp` with the same unnamed reads instead gives a peeled
  cmp-mem loop + fresh post-loop load — the direct-global spelling is load-bearing. Route any
  read that must NOT CSE (e.g. a fresh post-call re-read at return) through a second alias
  symbol. (0x28628 MATCHED 135/135.)
- **Whole-index named temp granularity (cont. 21 retry, 0x16318)** — when a `tbl[i + g*19]` chain
  has the multiply/index register roles rotated (plus a `mov edx,ebx; xor edx,ebx` zeroing quirk,
  +2B), a block-scoped `unsigned int idx = g * 19 + i;` used at both sites homes the index in EAX
  and the multiply chain in EDX like the target. Granularity matters BOTH ways: `t = i;` alone
  spills t and wrecks EDI; casts and +-commutes are byte-inert. Closed the 0x16318 loop window
  (242→280/287; residue is a 2-instruction entry-order swap, entry-scheduler wall family).
- **Byte-cast forces byte test (cont. 21)** — `(unsigned char)(node[0xb] & t)` emits the byte
  `test [eax+0xb],dl`; without the cast Watcom zero-extends both operands and emits a dword test.
  (0x128b8)
- **Negative guards preserve cmp orientation (cont. 21)** — an `&&` chain makes Watcom
  canonicalize `A >= B` into a reversed `cmp B,A / jg`; writing `if (A < B) goto skip;` keeps the
  target's `cmp A,B / jl` orientation. (0x128b8 box checks)
- **`volatile int x` = slot-homed local (cont. 21)** — when the target keeps a hot local in a
  stack slot with EVERY access through memory (load-op-store per statement, slot reload at each
  push) while ours registers it, `volatile` on that one local reproduces the slot-homed form
  exactly without perturbing neighbours. (0x37918's x; note volatile on a PARAM was worse —
  0x35d08.)
- **Named base pointer for g_5358 column lookups (cont. 21)** — `base = g_5358; slot = base +
  idx;` forces the `a1` base load BEFORE the `lea reg,[reg*4]` index scale; plain `&g_5358[idx]`
  was anti-correlated with an unrelated block's codegen (fixing one flipped the other — the named
  base pinned both). Plus `(int)*slot` casts the tile deref into `add eax,[ebp]; cmp [eax]`
  instead of the SIB-folded form. Matched 0x2fca8 (438B); likely un-parks the 0x2d5b8/0x28ec8
  g_5358 column register wall — RETRY those with this lever.
- **Structured-loop-var slot rank vs loop layout (cont. 21, wall data)** — a real
  `for(i=0;s[i];++i)` header BOOSTS i's spill-slot rank (landing it at [esp+0]) but -oneatx then
  emits jump-to-test layout; `for(;;)`+break gives test-at-top but demotes the slot. The two
  co-vary in opposite directions — when a target needs both, that's a wall (0x36698). Also: y
  param as `unsigned short` explains dword slot loads with dirty upper + `xor ah,ah` half-widen
  on `y += a4`.
- **`-or` for slot re-read functions (cont. 21)** — a fn whose params/locals are re-read from
  stack slots at each use (no callee-saved homing, no extra push) wants `-4s -or -zp8 -s -zq`;
  -oneatx homes them into EBX/EBP and adds a push. Confirms the 0x377b8 recipe row. (0x284a8
  MATCHED this way.)

- **i86.h FP_SEG pragma = dead offset load + `mov dx,cs` (cont. 22, MATCHED 0x28b88)** — the
  pair `mov eax,<fn-addr>; mov dx,cs` with EAX never used is the Watcom 10a i86.h `FP_SEG`
  (`#pragma aux FP_SEG = parm caller [eax dx] value [dx];` — an EMPTY body; the far cast of a
  near function pointer materialises off in EAX, CS in DX). Transcribe the extern+pragma inline
  (sources have no includes) and call `FP_SEG((void __far *)FUN_xxx)`. Companion in the same fn:
  `in.edx = (int)FUN_xxx;` gives the reloc'd `mov edx,imm32` FP_OFF form.
- **Call-per-switch-case beats size-temp + single call (cont. 22, 0x28b88)** — when each case
  body is `push imm32; jmp SHARED_CALL` (68 xx), the source calls the function INSIDE every case
  (`case 1: g = malloc(0x1100); break; …`) and Watcom tail-merges the call+store; a switch-assigned
  size temp + one call after emits `mov eax,imm` per case + ONE shared push (+1B).
- **Post-store global null-test re-read (cont. 22, 0x28b88)** — `g = f(sz); if (g != 0)` CSEs to
  `test eax,eax`; the target's `mov [g],eax; cmp dword [g],0` needs the volatile-alias extern
  lever (second extern name `void * volatile g_v;` tested instead) — confirms cont.21 entry.
- **Statement-order write of a hoisted block constant (cont. 22, 0x28b88)** — in a store-block
  feeding int386x, the constant whose STORE the target sinks LAST but LOADS first into a
  callee-saved reg (edi=0xc at block top, `mov [in.ax],di` after the far-cast) is the FIRST
  statement of the block in source; written last it degrades to an immediate word store.

## 3. Walls (recognize, then PARK — not source-reachable)

If the structure is byte-correct and only ONE of these remains, stop and park with a note. Do not
grind; the fuzzer permutes *source* and can't change the allocator's mind.

**WALL TAXONOMY (cont. 25 — investigated the "register-search" question; this is the honest map).**
The parked near-misses split into THREE classes, and only one is even theoretically source-reachable:
1. **Encoding tie-break (NOT register — most common, misfiled for ages).** Watcom's instruction
   ENCODER, not its allocator, picks the byte form: accumulator `05 imm32` vs `83 c0 imm8` for
   `add eax,imm` (asymmetric with the `sub` path even in the SAME fn), `xor dh,ah` cross-byte vs
   `xor dh,dh` self-zero, push imm8-vs-imm32. **0x34048's register role was actually SOLVED** — its
   residual is pure encoding. NO source transform and NO register search can move these; the encoder
   is fixed. Verified: 20000 combined cpermute variants (incl. decl-perm) leave 0x34048 at 50/56.
2. **Accumulator-selection register tie.** `add edx,eax` vs `add eax,edx` for `mem + reg` — which
   reg is the accumulator holding the running sum. Commutative swap RE-canonicalises (often worse);
   no C construct forces the accumulator. Some are permuter-reachable when a swap happens to flip it
   (0x272b8 cracked), most are not (0x2d0d8 — same class, not reachable). 
3. **Per-body register-pressure VARIATION.** One fn reads the same value 2-3 different ways in
   different bodies (`movsx eax/edx,[edx/eax+0xb6]` ×3 in 0x23158) because each body's surrounding
   pressure differs. Inherently NOT reproducible from a uniform C spelling — it reflects the
   original's per-body source structure we can't recover. Whole template family (0x223c8, 0x12da8,
   0x23158) parked here.
CONCLUSION: a "register-allocation-aware search" targets only class 2, and even there the
accumulator choice isn't a source-level knob. The decl-perm tool (cont. 24) was the reachable
slice; going further is against the evidence. PARK these, don't build for them.

- **Register-role tie-break** — identical instructions, but a value lives in a different register
  (EAX vs EDX / ESI vs EBX), cascading into encodings (`cwde`↔`movsx`, `lea`↔`add`, `test al`↔`test
  dl`, `cmp ax`↔`cmp dx`). Every C spelling + the fuzzer converge to the wrong reg. (0x34048, 0x34088
  CMP-modrm, 0x2dd48, 0x36d18, 0x183e8 via LICM; 0x19318 — one post-call slot-reload pick, d into
  EAX vs target ESI, rotates every later reload, kills a branch-tail cross-jump (+4B) and costs a
  4th spill-temp slot (+4B frame); init-order permutations, named temps, int dword-alias reload,
  for-header all fail or regress. Parked 589/585.)
- **uchar widen-form (xor-first vs mov+and)** — widening a byte for an int arg: a NAMED char local
  gets `xor eax,eax; mov al,dl` (5B); a compiler CSE temp gets `mov al,ch; and eax,0xff` (8B,
  and-form). NO LONGER a hard wall when the value comes from MEMORY: respell the source so the
  value is never named — read it inline through the global at every use (see the cont.21-retry
  anonymous-global-deref lever in §2; 0x28628 MATCHED this way). Still a wall only if the value is
  genuinely computed/loop-carried with no memory home to re-read.
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
- **Scaled-index lea materialization (cont. 21)** — target materialises `2*minor` as the 7-byte
  `lea eax,[esi*2+0x0]` (disp32=0, NO fixup); our 9.5b always emits `add eax,eax` (or mov+add).
  `*2`, `<<1`, `(short*)0+dy`, and extern-symbol-base spellings all fail (symbol forms don't fold
  sym+idx*2 into one lea and restructure the entry). Same codegen-choice class as the push
  imm8-vs-imm32 wall below. (0x18ae8 Bresenham, 516/524.)
- **Spill-slot steering refinement (cont. 21)** — decl-order slot steering is NOT universally
  inert: in 0x18ae8 endpoint/step locals were successfully steered to the target's slots via
  declaration order, while its inc-quartet stayed in a 3-cycle and 0x338d8/0x12ae8's counters
  ignored decls entirely. Try decl-order steering first; park only when the residue is a cyclic
  permutation that decls provably don't move. Also (0x12ae8): inline `node[0x18]` (vs a named
  temp) homes the loaded type byte in AH (`cmp ah,1`) instead of AL — another inline-vs-named
  data point.
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

- **Spill-slot assignment order (cont. 21)** — when 3+ byte locals spill to [esp+0/4/8/0xc], the
  ORDER they get slots is allocator-internal: ours sorts role-first (all outer loop counters low,
  all inners high) whatever the decl order (decl-order flips are provably byte-inert), scoping
  (block vs function-top), or variable reuse across loops (perturbs order but never reaches the
  target's case-grouped interleave). A [esp+0]-vs-[esp+4] disp8 difference shifts 1 byte and every
  loop-alignment pad after it. 10 configs tried. Same family as register-role. (0x338d8 — body
  otherwise byte-correct.)
- **Param auto-promotion to callee-saved reg (cont. 21)** — ours promotes a stack param into a
  callee-saved register at entry (profitable: 3 uses across calls) where the target keeps it
  memory-homed until first use; ushort/int/short typings, casts, volatile (worse), and
  assignment-in-condition all fail to suppress it. Companion: ours tail-MERGES identical 7-byte
  `xor eax,eax; pops; ret` guards into one far block where the target duplicates them inline —
  also not steerable. (0x35d08 338/346.)
- **Loop-split march: gap-hoist vs anti-interleave are mutually exclusive (cont. 25)** — a
  fn with TWO byte-identical physical copies of a march loop (phase-1 with a trailing collision
  CALL, phase-2 a plain continuation after the break) where each copy's accumulator load must be
  hoisted into a `shl/sar` latency gap (`shl edx,8; movsx eax,g; sar edx,8`). Phase-2 (no trailing
  call) gets the clean per-statement schedule from a plain non-volatile `+=` and matches exactly;
  phase-1's trailing call frees the scratch regs and lets -oneatx INTERLEAVE the two marches
  (loads the 2nd table straight into the index reg, `movsx edx,[edx*2+t]` 8B, vs `movsx eax; mov
  edx,eax` 10B — 2B short). `volatile` on the accumulator global blocks the interleave (restores
  length + the `mov edx,eax` form) but, as a hard scheduling barrier, PINS the load to program
  order AFTER `sar` (one instr late) in BOTH copies. So you get either hoisted-but-interleaved
  (plain +=) or barrier-pinned-late (volatile) — never both, because the gap-fill needs a
  reorderable load and anti-interleave needs a barrier. A volatile-ALIAS used in only one copy does
  NOT work (the barrier must be the SAME symbol in both copies to interact with the call's read of
  it). Levers that DID land here: callee param `short` for the `xor;mov al;cwde` arg widen; tail
  floor compare spelled `a < b/2` for the `cmp edx,eax; jge` orientation. (0x34198 PARKED 453/453B
  95.4%, first diff 0x77; two-loop source, phase-2 exact. Same class as 0x34608's cross-jump wall.)
- **Entry-scheduler load batching (cont. 21)** — ours batches all entry loads (param + globals)
  at the top and loads a byte global into AH for a `test` where the target compares memory
  directly (`cmp byte [mem],0`) mid-sequence; ours also value-numbers a loop's `-1` into CH
  (`add al,ch`) where the target has `dec al` — the -oneatx const-hoist. Named-local and spelling
  changes don't break either; cpermute 4000 variants no match. (0x264a8 314/297.)

## 4. Object model (see docs/object-model.md)

Pool A entity record (92B, `node = g_810e + id`, links are 16-bit ids not pointers): coords s16 at
`+4/+6/+8`, flags `+0xa/+0xb`, type/frame `+0x18/+0x19`, facing `+0x1a`, links `+0/+2/+0x1c/+0x24`,
health `+0x14`(word)/`+0x54`(byte). Direction tables `g_ab60/g_ad60` (s16[256]); grid `g_10e`
(u16[128*128]); screen buffers `g_5368/g_5370`. `(short)ptr + 0x7ef2` and `ptr - g_810e` are the
ptr↔id idioms.

---
*Changelog: created cont.14 from the parallel-agent run. Append new levers/walls here as found.*
