# Porting this method to another game — a matching-decompilation field guide

This is the **game-agnostic** distillation of everything we learned byte-matching *Syndicate*
(1993/95 DOS, Watcom C/C++ 9.5). If you want to start a matching decompilation of a *different*
period binary, read this first: it is the method, the toolchain shape, the permuter design, the
lever catalogue, and — most importantly — the **wall taxonomy** that tells you when to stop.

The Syndicate-specific reference is [`matching-playbook.md`](matching-playbook). This page is the
part that transfers.

---

## 0. What "matching" means and whether you want it

A **matching** (byte-exact) decompilation produces C source that, compiled with the *original
period toolchain*, reproduces the shipped machine code **byte for byte**. It is very different from
a **behavioural** reimplementation (which just has to act the same). Matching is a forensic,
convergent activity: there is a single right answer for each function and a mechanical oracle
(`compiled bytes == target bytes`) that tells you when you have it.

Use matching when you want a **provably faithful** source recovery, a regression-proof base for
porting, or simply to learn how a compiler thinks. Do **not** use it if you only need the game to
run — that is 10× less work as a behavioural port.

The single most important early decision: **relocation-aware match, not raw byte-equality.** Linked
addresses (call targets, global references) differ between your object and the shipped image because
the linker fills them. Mask exactly those bytes on both sides (from the object's own relocation
records) and require equality of everything else. Everything below assumes that oracle.

---

## 1. The pipeline (what to build first)

Independent of game, you need six pieces, in order:

1. **A linear image of the target code segment.** Extract the executable's code into a flat
   `linear.bin` addressed so that `linear.bin[A - image_base]` is the byte at virtual address `A`.
   For DOS LE/LX (DOS/4GW) that means un-relocating object1; for other formats, the equivalent.
2. **A function inventory** (`manifest`): one record per function — address, size, status. **Size
   is the recurring off-by-one trap:** disassembler "body end" is usually *inclusive* (`size =
   end - start + 1`), and gap-based sizing (next start − this start) **includes trailing
   alignment padding**, which is wrong for clean-C matching. Get true sizes from the disassembler's
   function boundaries, not gaps.
3. **A containerised period compiler** (see §2). One clean, reproducible invocation:
   `source.c → object` with a fixed flag recipe.
4. **A relocation parser** for your object format (OMF for Watcom/DOS; COFF/ELF elsewhere) that
   yields `(offset, size)` of every fixup. This drives the masking. Ours is `tools/omf.py`.
5. **The match oracle**: compile → read `_TEXT` bytes + fixups → mask both sides → compare
   (`tools/match_reloc.py`). Output is a hard `YES/NO` plus the first differing offset.
6. **A whole-program build** (optional but valuable): link every object into a runnable image.
   This catches "my function is right but its *contract* is wrong" and lets you actually run what
   you've recovered. It also *forces* you to name every symbol, which surfaces mistakes.

With those, the loop per function is: read the disassembly → write C → compile → diff → iterate.

---

## 2. Pinning the compiler (do this before you write any C)

**You cannot match code without the exact compiler.** A different major version, or sometimes even
a patch level, changes register allocation and peephole encoding. Our compiler hunt
([`compiler-version.md`](compiler-version)) is the cautionary tale worth internalising:

- **Identify the family from the runtime.** The C runtime library (CLIB) usually carries a version
  banner/copyright string and has recognisable, hand-written helper routines. Match *those* first —
  they pin the family precisely (we proved Watcom 9.5 small-model `CLIB3S` this way).
- **Confirm by bulk match.** Compile a few dozen simple functions. If ~all of them match, you have
  the right family. If most *don't*, you have the wrong compiler — stop and fix that before
  grinding individual functions. (We match 439 functions with 9.5b; that is the real proof.)
- **Beware the "between versions" trap.** A binary can match family *N* on the bulk yet exhibit a
  handful of features from version *N+1* (we see 9.5 register allocation but occasional **10.0-era
  `add eax, imm32` accumulator encodings**). This usually means the true build is a **transitional
  patch/OEM release** that may not be publicly archived. Bracket it (`9.5b < game < 10.0a` for us)
  and don't waste weeks: if the exact build is lost media, bank the matchable majority.
- **Test versions cheaply.** Build a *compiler sweep* (`tools/compsweep.py`): hold C + flags fixed,
  swap only the compiler, compile the hard functions, report which version matches. Run it on the
  **walls**, not random functions — the walls are where versions diverge. Our sweep spans 7 Watcom
  builds; the walls are *invariant* across all four 9.5 point releases, which is itself the proof
  they are register-allocation ties and not a version artefact.
- **Direction matters.** If a wall needs a *newer* encoding (imm32 accumulator form, address
  unfolding), an *older* compiler cannot help — it will only regress the bulk. We reasoned 8.5 out
  on exactly this basis before spending any download effort.

Containerise the compiler for reproducibility. Period DOS compilers run fine under **DOSBox**
(pure emulation). Resist the temptation to use a KVM-backed DOS emulator at high parallelism — many
concurrent micro-VMs can crash the host VM. DOSBox + modest concurrency + memory caps is safe.

---

## 3. The permuter (how automated search fits in)

Hand-iteration finds most matches; a **permuter** closes the near-misses and, crucially, *proves*
when a residue is a genuine wall rather than unfound C. Ours is `tools/cpermute.py`. The design
principles transfer:

- **Permute the source, not the bytes.** The search space is *semantically-equivalent C spellings*
  that the compiler lowers differently: declaration order, statement order, loop form, sub-expression
  association, explicit-temp vs inline. Each candidate must still compile and still be correct.
- **Declaration-order permutation is the highest-value lever.** Compilers assign stack slots (and
  often registers) in declaration order. Permuting the top-level declaration block — exhaustively
  for ≤7 locals, sampled beyond — settles most spill-slot/offset near-misses. Watcom lays locals
  out in *reverse* of declaration; other compilers differ, but *some* deterministic order exists and
  permuting it is mechanical.
- **Know what decl-perm canNOT move.** It shifts straight-line slot-homed locals; it does **not**
  move loop-carried values or pure register-role ties. If a residue survives *all* declaration
  permutations, it is a genuine allocator wall (§4) — **stop hand-grinding.** This "prove it's a
  wall" function is the permuter's most valuable output, more than the matches it finds.
- **Make it structure-aware.** A `switch` co-locates its jump table inside the function's `_TEXT`;
  the shipped binary keeps the table elsewhere. Teach the scorer to split the table off before
  comparing, or dispatchers will look like near-misses forever.
- **Stack-slot padding as an explicit lever.** decomp-permuter's `perm_pad_var_decl` idea (inject a
  dummy local to shift the frame) is worth having; we ported it. It occasionally shifts an offset
  that nothing else reaches.
- **Run it as a *triage* tool.** Score every parked function; the ones that plateau at N−1 of N with
  a single-instruction residue are your crack candidates. The ones that plateau far off are deep
  register-role walls — deprioritise them.

A permuter's job is not just to find matches; it is to **partition** the parked set into
"reachable-with-more-search" and "genuine wall" so humans spend time only on the former.

---

## 4. The wall taxonomy (recognise, then park)

This is the payoff of the whole project: knowing the *shape* of an unmatchable difference so you
don't waste days on it. After the reloc-aware diff, filter out relocation artefacts (see the trap
below) and classify what remains:

| Wall | Signature in the diff | Source-reachable? |
|---|---|---|
| **Register-role** | Same instructions, a register substituted throughout (ESI↔EDI, EAX↔ECX); often a callee-saved reg pushed on one side | **No.** The compiler's allocator picked it; no C spelling flips it. Dominant wall. |
| **Instruction scheduling** | Same instructions, two independent ones reordered (e.g. the `[esp+4]` load always precedes `[esp+8]`) | **No.** The scheduler is deterministic and order-fixed. |
| **Encoding tie-break** | Same operation, different bytes: `cmp a,b` vs `cmp b,a` (modrm order); `imm8` vs `imm32`; `xor dh,dh` vs `xor dh,ah` (clear via a known-equal reg) | **No** for a given compiler; sometimes flips between *versions* (test with the sweep). |
| **Value-tracking / block-duplication** | One side re-materialises a constant (`mov eax,-1; ret`) that the other proves already-in-register and shares | **No.** Defeating the compiler's value analysis costs other bytes. |
| **Dead callee-save** | Target wraps the whole body in a `push ebx … pop ebx` that is never used | **YES — crackable, see §5.** |
| **Cross-function tail-merge** | A jump lands *outside* the function, into a *neighbour's* return stub | **YES — crackable, see §5.** |
| **Alignment/scheduling nops** | Extra `lea eax,[eax]` / `mov ebx,ebx` no-ops mid-body | Rarely reachable; placement depends on absolute layout. |

**The relocation-artefact trap (critical for triage tooling).** Your freshly-compiled object has
*unresolved* relocations — a global reference shows as `[0]` or an omitted displacement, an `add
eax, 0`. The shipped image has them *resolved* (`[0x10b4a]`, `add eax, 0x810e`). A naive instruction
differ counts these as differences and **massively over-ranks** global-heavy functions as "far from
matching." Mask them: for *your* side use the object's fixup offsets to blank the reloc'd operands;
for the *target* mask absolute addresses (large constants). We got this wrong first and a function
that was **1 instruction from matching ranked as 9** until we fixed it. Trust the reloc-aware oracle,
not a raw instruction differ, for closeness.

**Rule of thumb:** if the diff is register-role or scheduling and survives declaration-permutation,
it is a wall — record the *purpose* of the function (that's the half that lasts) and move on.

---

## 5. The cracks — walls that look irreducible but aren't

Two wall classes that *seem* like register-allocation magic have a real C/toolchain lever. These are
the highest-leverage discoveries; check for them before parking.

### Dead callee-save (`#pragma aux <callee> modify [reg]`)
A tiny function whose body compiles to a bare guarded call, but the target wraps it in a
**dead `push ebx … pop ebx`** (ebx never used). Mechanism: the function must preserve `ebx` for
*its own* caller (callee-saved contract); if one of *its callees* is declared to clobber `ebx`, the
compiler saves/restores `ebx` around the whole body to honour that contract — even though nothing
local touches it. Declaring the callee `#pragma aux <callee> modify [ebx]` (which reflects the
original translation unit's header ABI, and only adds `ebx` to the clobber set — it does *not* change
argument passing) reproduces the phantom save. It is the **only** C construction that yields those
bytes; any real use of `ebx` emits extra loads/stores. The equivalent on other toolchains is
whatever mechanism declares a callee's clobbered-register set.

### Cross-function tail-merge (whole-module build)
A function whose `return 0` (or any tail) has **no local stub** — it jumps *backward into a
neighbouring function's* identical return stub. That cross-function branch only exists when both
functions are compiled in **one translation unit** (the compiler's cross-jump/tail-merge optimisation
is intra-object). To reproduce it:
1. Put both functions in **one source file**, in **address order** (the stub-owner first).
2. Make the tails **byte-identical** so the compiler can share them — for us that meant changing the
   borrower's return type from `unsigned char` to `unsigned short`, so its return-0 stub matched the
   owner's `xor eax,eax; ret` exactly.
3. Verify the **whole contiguous region** (both functions + inter-function padding) reloc-aware, so
   the cross-function branch is checked byte-exact (it is *not* a relocation). Our verifier is
   `tools/match_combo.py`; a manifest `unit` field tags which functions share a file.

Scan for these systematically: disassemble each parked function and flag any `jcc/jmp` whose target
lands **outside** its own `[addr, addr+size)` range (`tools/xfnjumps.py`). In Syndicate this vein was
a single function — but on a game built from larger modules it could be many.

---

## 6. The lever catalogue (steer codegen from C)

Before parking a near-miss as a wall, try the levers that *do* change output (full list in
[`matching-playbook.md`](matching-playbook) §2). The transferable ones:

- **Return width.** `int` vs `short` vs `char` return changes the exit widening (`xor ah,ah`,
  `cwde`) and, as above, whether tails can be shared.
- **Signed/unsigned and cast width.** Controls `movsx` vs `movzx` vs `and eax,0xffff`, and 16- vs
  32-bit loads (`mov ax` vs `mov eax`).
- **Loop form.** `while` vs `do-while` vs `for` decides whether the compiler *rotates* the loop.
  A `while` that should be a `do-while` masquerades as needing heavier optimisation — it doesn't,
  it needs the right loop shape. (This one bit us hard; see the journal.)
- **Named temp vs inlined expression.** Naming a value forces it to persist (often in a
  callee-saved register across a call); inlining lets the compiler fold it. The diff tells you
  which: a stray copy says inline it; a value that should survive a call says name it.
- **Declaration order.** Stack-slot and often register assignment; see the permuter.
- **Address formation.** `arr[i].field` (base + index + displacement) vs `&arr[i]` then `p->field`
  (folded base) produce different addressing modes. Match the target's.
- **Symbols, not literals.** Write `&g_11670` / `g_base + 0x9562` the way the original did — the
  linker folds constant + relocation, and the *addend* is what appears in your object.

---

## 7. When to stop (the floor)

A mature matching project converges to a **floor**: a set of functions that are decoded, understood,
and byte-exact in the build via a fallback, but whose *clean C* cannot reproduce the exact bytes
because the remaining differences are compiler-internal (register/schedule/encoding) or need a
compiler build you don't have. Recognise the floor honestly:

- Every close candidate, when diffed cleanly, is a documented wall type from §4.
- The compiler sweep shows the walls are version-invariant across every build you can obtain.
- The permuter plateaus one instruction short with a register-role or scheduling residue.

At the floor, the right move is a **byte-exact fallback** for the holdouts so the whole image still
reproduces: transcribe the raw bytes as inline-asm/`db` pragmas (like N64 projects'
`asm/nonmatchings/`), verified to reproduce the image. Keep the readable near-miss C alongside as the
documentation of intent. Coverage of *bytes* stays 100%; coverage of *clean C* is whatever the
compiler allows. Don't grind register walls past this point — record the function's purpose (the
durable half) and stop.

---

## 8. Tooling manifest (what each piece does)

The Syndicate tools, named so you can build equivalents:

| Tool | Role |
|---|---|
| `omf.py` | Parse object format → `_TEXT` bytes + relocation `(offset,size)` list |
| `match_reloc.py` | The oracle: compile → mask both sides → `YES/NO` + first diff |
| `match_combo.py` | Whole-unit oracle for multi-function files (cross-fn tail-merge) |
| `cpermute.py` | Source permuter (decl/statement/loop/pad), jump-table-aware, triage mode |
| `compsweep.py` | Compiler-version sweep (fixed C+flags, swap compiler) |
| `flagsweep.py` | Compiler-flag sweep (fixed C+compiler, swap flags) |
| `crackreg.py` | Reloc-aware instruction differ + distance ranking + `--diff` view |
| `xfnjumps.py` | Scan parked fns for cross-function branches (tail-merge candidates) |
| `crackfix.py` | Batch-apply the `modify [reg]` dead-callee-save fix |
| `dbgen.py` / `unbake.py` | Byte-exact `db`-transcription fallback + verify-against-image |
| `buildgame.py` | Link every object into a runnable image (forces symbol completeness) |
| `treemap.py` / `progress.py` | Visualise coverage by subsystem and over time |

The order to build them: oracle first (`omf` + `match_reloc`), then the manifest, then the permuter,
then the sweeps, then the reloc-aware differ, then the fallback. Everything else is game-specific.
