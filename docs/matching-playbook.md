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
- **If match95 fails only on a length check**, the manifest `size` may be wrong (headless pass
  under-counted; e.g. 0x29c58 was 177 vs true 238). Confirm the extent with `get_function_by_address`
  and correct the manifest `size`.

## 1. Recipes (compile flags)

| recipe | when |
|--------|------|
| `-4s -oneatx -zp8 -s -zq` | **default** — stack-calling game code (< 0x39000) |
| `-4r -oneatx -zp8 -s -zq` | `__fastcall`/register-calling fns; the `0x39xxx` region |
| `-3s -of -oneatx -zp8 -s -zq` | framed runtime-library region (`0x3a000+`, non-leaf) |
| `-4s -ot -s -zq` (or `-oe/-or/-os`) | the occasional less-optimised unit (target bigger/un-folded than -oneatx) |

Determine the convention from the disasm: params from `[ESP+..]` → `-4s`; params from ECX/EDX →
`-4r`. When unsure, try both.

## 2. Levers (steer Watcom from C — these CLOSE near-misses)

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
- **Signed vs unsigned `char`** — Watcom `char` is UNSIGNED (a `char` compare emits `JB`/`JAE`).
  Use `(signed char)` to get the target's signed `JL`/`JGE`. (0x22ba8)
- **`#pragma aux <FnName> modify [eax ecx edx ebx];`** — puts a callee-saved reg in the volatile
  set, suppressing a spurious `push/pop ebx` when the target uses EBX as unpreserved scratch. (0x180f8)
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
- **Intrinsic won't inline** — target uses `rep movs`/`repne scasb` (inlined memcpy/strlen); our
  batch compile has no `<string.h>` on the include path and neither `-oi` nor `#pragma intrinsic`
  triggers it. (0x299c8, 0x17998.)
- **Scheduling / param-load order** — high-param-pressure fns where the exact instruction schedule
  (which param loads first) isn't source-reachable. (0x15e38.)

## 4. Object model (see docs/object-model.md)

Pool A entity record (92B, `node = g_810e + id`, links are 16-bit ids not pointers): coords s16 at
`+4/+6/+8`, flags `+0xa/+0xb`, type/frame `+0x18/+0x19`, facing `+0x1a`, links `+0/+2/+0x1c/+0x24`,
health `+0x14`(word)/`+0x54`(byte). Direction tables `g_ab60/g_ad60` (s16[256]); grid `g_10e`
(u16[128*128]); screen buffers `g_5368/g_5370`. `(short)ptr + 0x7ef2` and `ptr - g_810e` are the
ptr↔id idioms.

---
*Changelog: created cont.14 from the parallel-agent run. Append new levers/walls here as found.*
