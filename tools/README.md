# tools — the active matching toolchain

Everything here operates on the **Watcom C/C++ 9.5b** pipeline (the confirmed period
compiler), run headless under DOSBox inside the `synd-decomp` container. Superseded and
one-off scripts live in [`archive/`](archive/README.md).

## The primary workflow

```
pick a target ─► write src/<FUNC>.c ─► match95.sh ─► exact?  ─► mark.py + recipes.json
   (navigation)     (by hand from        (compile      │ near-miss
                     the disassembly)      + diff)      ▼
                                                    cpermute.py   (the fuzzer:
                                                    mutate the seed until the
                                                    bytes match, or prove a wall)
```

Then `regress95.py` keeps every recorded match green.

## Core tools

### The fuzzer (primary — read the header of `cpermute.py` in full)
- **cpermute.py** — AST-based C permuter. Parses `src/<FUNC>.c`, applies randomized
  semantics-preserving mutations (loop-form, commutative swap, strength-reduction/relational/
  range-check rewrites, type permutation, temp introduction, **variable inlining**, statement
  reorder), batch-compiles hundreds/sec, and keeps the variant whose bytes match. Writes the
  winner to `src/<FUNC>.c.match`. When nothing matches, its best near-miss + source pinpoints
  the exact compiler-choice divergence — often the signal that a target is a genuine wall.
  This is the tool we lean on; it is heavily commented for that reason.

### Compile + diff pipeline
- **match95.sh** — match one function end to end: `wcc_95.sh` then `match_reloc.py`.
  `docker run --rm -v "$PWD":/work -w /work synd-decomp bash tools/match95.sh <FUNC> "<flags>"`
- **wcc_95.sh** — compile `src/<FUNC>.c` → `build/<FUNC>.obj` with WCC386 (Watcom 9.5b).
- **wcc95_batch.sh** — compile every `SRC*.C` in a workdir to `O*.OBJ` in ONE DOSBox session
  (amortises the ~0.85s cold start). The speed engine `cpermute.py` runs on.
- **match_reloc.py** — relocation-aware diff of our object vs the target bytes (masks fixups).
- **omf.py** — parse a Watcom OMF `.OBJ`: returns `.text` bytes + fixup ranges. Shared by
  `cpermute.py` and `match_reloc.py`.
- **regress95.py** — recompile every function in `manifest/recipes.json` and assert each still
  matches. Run after any change that could affect matched output.

### Recipes & optimisation classification
- **recipes.py** — derive and record the compile flags for each matched function into
  `manifest/recipes.json` (the regression baseline).
- **optclass.py** — for each matched function, hold its calling convention fixed and find which
  optimisation level it actually needs (spots wrong-C / lighter-unit suspects).
- **caltest.py** — calibrate a candidate flag bundle against the whole regression baseline.
- **mark.py** — flip a function to `matched` in `manifest/functions.json`.

### Navigation / triage (finding the next target)
- **nextgame.py** — unmatched **game** functions (addr < 0x3a000), by size, with first bytes.
  The real targets — the 0x3a000+ region is the pre-compiled Watcom C runtime (see AGENTS.md).
- **pick.py** — small, low-call, low-data unmatched candidates (easy wins).
- **smallest.py** — smallest unmatched functions.
- **framed.py** — unmatched functions with a `55 89 e5` frame prologue (the framed class).
- **byprefix.py** — unmatched functions whose bytes start with a given hex prefix.
- **triage.py** — histogram of unmatched functions by opening-byte pattern/shape.
- **callgraph.py** — static call graph (E8/E9 rel32 → known function starts) for top-down nav.

### Library identification (the 0x3a000+ runtime region)
- **libmatch.py** — find which framed functions are library code by searching the Watcom
  `.LIB` files for their exact bytes.
- **libname.py** — name those library functions by the containing `.LIB` module (`wlib`).

### Reporting
- **score.py** — completion score; byte coverage (matched machine-code bytes / total) is the
  honest headline, function count is shown too.
- **dashboard.py** — regenerate the `dashboard/` progress view.

### MCP
- **bridge_mcp_ghidra.py** — the Ghidra MCP bridge server (launched via `.mcp.json`). Not part
  of the matching loop, but the disassembly/analysis backend behind it.
