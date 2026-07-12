# Watcom 9.5b build+test pipeline (compiler-version experiment)

Proves/tests whether a patched Watcom 9.5x reproduces the game's codegen (see
`docs/compiler-version.md`). The period patch binaries live under the git-ignored
`toolchain/w95b_dl/` (downloaded from archive.org: base-9.5 floppies `W9532_*.img`
and the 9.5b patch `Patch32.zip`).

- `build95b.sh`  — bpatch our base-9.5 `WCC386.EXE` (627702B) with `WCC386D.A`+`.B`
                   → 9.5b compiler at `toolchain/watcom95b/BIN/WCC386.EXE`.
- `test95b.sh <FUN> "<flags>"` — compile one src with the 9.5b compiler under DOSBox.
- `cmp95b.sh <FUN>...` — compile + masked-compare against the target bytes.

RESULT: 9.5b ruled out (walls persist). Reuse this to test 9.5c if it surfaces:
drop its patch into toolchain/w95b_dl and point build95b.sh at it.
