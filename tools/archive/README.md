# tools/archive — superseded & one-off tooling

These scripts were part of getting the project off the ground but are no longer in the
active loop. Kept (not deleted) for reference and in case a technique needs revisiting.
Nothing in the active pipeline imports or calls anything here.

## Superseded permuters (replaced by `tools/cpermute.py`)
The fuzzer went through three earlier forms before the AST-based `cpermute.py`:

- **permute.py** — single-thread permuter driven by an inline template with `$[ a $| b $]`
  alternatives written into the source by hand.
- **permute_c.py** — the same manual-template idea, batched, reading a template file from
  `archive/tmpl/`. (`tmpl/FUN_00033fb8.c` is its one surviving template.)
- **permute_par.py** — parallel *case-order* search. Its notable result: it exhausted all
  40,320 orderings of the `0x20d98` switch in ~3m9s to prove that target size (342B) is
  unreachable by reordering. The batched-DOSBox speed trick it pioneered now lives in
  `wcc95_batch.sh`, which `cpermute.py` uses.
- **wcc95_file.sh** — isolated-workdir single-file compile driver used only by
  `permute_par.py`.

`cpermute.py` subsumes all of these: it mutates a real pycparser AST (no hand-written
templates), covers more transform families, and scores alignment-aware.

## Old Watcom-10.0a / DOS matching pipeline (replaced by the 9.5b pipeline)
Before we confirmed the game was built with **Watcom C/C++ 9.5b**, matching went through
Watcom 10.0a. Superseded by `match95.sh` / `wcc_95.sh`:

- **match.py** — the original single-function matcher (Watcom 10.0a).
- **match10.sh** — match-one wrapper for the W10 compiler.
- **wcc_dos.sh**, **wcc_la.sh**, **wcompile.sh** — W10/DOS compile drivers and variants.
- **batch_match.py** — swept unmatched functions for mechanically-generatable patterns (W10).
- **batch_stubs.py** — batch-matched the 10-byte `mov eax,imm; jmp rel` dispatch-stub cluster (W10).

## One-off setup / unpacking
Run once while bootstrapping the repo; not needed for ongoing work:

- **le_hdrdump.py** — dump the LE (Linear Executable) header of `SYNDICAT_MAIN.EXE`.
- **le_fixups.py** — parse the LE fixup (relocation) records to build the linear image.
- **wunpack95.sh** — unpack the period Watcom 9.5b toolchain into `toolchain/`.
