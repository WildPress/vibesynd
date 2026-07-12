# Compiler-version investigation (the wall on reaching 500/500)

## Conclusion

The game's **RUNTIME LIBRARY** is Watcom C/C++ **9.5, small-model CLIB3S** (proven byte-identical
by `tools/libname.py` — see the RTL work). But the game's **CODE** was compiled by a **different
9.5-branch build** of `wcc386` than the base 9.5 we have. Our compiler makes systematically
different codegen choices that **no source form or `-o` flag can override**, so ~50% of the
remaining functions are byte-identical to the target *except* for these choices and cannot be
matched with our current `wcc386`.

## The divergent codegen (the "walls"), each confirmed on multiple functions

| our base 9.5 wcc386 | the game's wcc386 | functions |
|---|---|---|
| fold address into SIB `[base+idx*N]` | callee-saved base + **un-folded** `lea;add;mov[reg]` | 0x269d8, 0x20d18, 0x35f78, 0x33b88 |
| `add eax,imm8` (`83 c0 20`) | accumulator `add eax,imm32` (`05 20 00 00 00`) | 0x34048, 0x16678 |
| `xor dh,dh` | `xor dh,ah` (cross-byte) | 0x34048 |
| duplicate epilogue | share one epilogue via `jmp` | ftell 0x3da03, fgetc 0x3d3e4 |
| accumulator → EDX | accumulator → EBX | atol 0x3a526 (confirmed by 33k-variant permuter) |
| left-operand → accumulator | reversed operand→register roles | 0x36648, 0x33b88 |

These are **Pentium-scheduling** characteristics (accumulator forms, address-unfolding to avoid
SIB/prefix pipeline penalties). The game shipped **Feb 1995** (Pentium era). Watcom **9.5c (1994)**
added Pentium tuning; our base **9.5 (1993)** predates it. Open Watcom v2 (container default) uses a
*third*, different allocator — not a match either. Proof it's not source-reachable: flag sweeps,
~35 hand-variants/function, and a 33,000-variant `cpermute` run all fail on the *same* byte.

## What we have / need

- **Ours:** `toolchain/watcom95` — banner **"WATCOM C32 Optimizing Compiler Version 9.5"** (base,
  no patch letter). `WCC386.EXE` is 627702 B (DOS/4GW-**bound**).
- **Patches exist:** 9.5a / 9.5b / 9.5c (9.5c = 1994, the Pentium one). Downloaded the **9.5b patch**
  (`toolchain/w95b_dl/Patch32.zip`, archive.org `watcom-9.5b`) + a base-9.5 patch set
  (`c32_*.zip`, archive.org `Watcom_C_9.5`). Both are `bpatch`-format patch sets, **not** base
  installs.
- **`bpatch` works:** `toolchain/watcom10a/WATCOM/BINB/BPATCH.EXE` (v1.3, DOS) runs under DOSBox.
  `Patch32/A/WCC3862.A` targets `wcc386.exe` and reports the **exact requirement**:

  ```
  Error! 'wcc386.exe' is the wrong size (627702) - should be (532992)
  ```

  So the 9.5b patch needs the **original 532992-byte base `wcc386.exe`** (the raw, un-bound
  compiler). We don't have that exact build.

## Remaining path to actually build the game's compiler

1. Get the **532992-byte base 9.5 `wcc386.exe`** — install base 9.5 from the floppy images
   (`W9532_01.img`..`W9532_10.img` on archive.org `Watcom_C_9.5`) via the DOS `SETUP` under DOSBox
   (interactive; the `.A` files are `bpatch`-compressed and need SETUP to expand).
2. `bpatch` it with `Patch32/A/*` then `Patch32/B/*` → **9.5b** `wcc386.exe`.
3. A/B-test 9.5b against the known walls (`0x20d18`, `0x34048`, `0x269d8`). If it reproduces the
   `05`/un-fold pattern, wire it up as `tools/wcc_95b.sh` and re-run the whole unmatched set.
4. If 9.5b's changes (mostly correctness fixes per its README) don't move the walls, hunt **9.5c**
   (the Pentium release) and repeat.

Until then: keep banking the ~50% of functions that don't trip these codegen paths; park the rest
as documented near-misses (register-role / address-fold / tail-merge / peephole walls, playbook §3).
