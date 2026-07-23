#!/bin/bash
# Proof that the WHOLE object1 blob + DGROUP data assemble and link as native Windows COFF.
# Extends win_rnc_proof.sh (one routine) to the entire code segment: generate the blob and the
# relocated DGROUP with the COFF leading-underscore convention, assemble both with the MinGW
# assembler, and link them together. A clean link with no undefined symbols means every one of
# the ~1854 code fixups + data pointers resolved under the Windows symbol model.
set -e
cd /work
[ -f build/obj1_full.bin ] || python3 tools/linearize.py >/dev/null

python3 tools/asm_emit_blob.py --underscore
python3 tools/port_dgroup.py  --underscore

AS=i686-w64-mingw32-as
LD=i686-w64-mingw32-ld
NM=i686-w64-mingw32-nm
gen=port/gen/blob

echo "assembling blob ($(wc -c < $gen/game_blob.s) bytes of .s)..."
"$AS" -o "$gen/game_blob.o"    "$gen/game_blob.s"
echo "assembling dgroup..."
"$AS" -o "$gen/dgroup_fixed.o" "$gen/dgroup_fixed.s"

# combine the two into one relocatable object; report any symbol still undefined
"$LD" -r -o "$gen/game_full.o" "$gen/game_blob.o" "$gen/dgroup_fixed.o"
echo "combined -> $gen/game_full.o"
und=$("$NM" "$gen/game_full.o" | grep ' U ' | grep -v ' U _shim_' || true)
if [ -n "$und" ]; then
    echo "UNRESOLVED symbols (excluding C shims):"; echo "$und"
    exit 1
fi
echo "BLOB-COFF-OK: whole blob + dgroup assemble and link as COFF, no undefined non-shim symbols"
"$NM" "$gen/game_full.o" | grep ' T _game_startup_main' || echo "(note: game_startup_main not found as a defined symbol)"
