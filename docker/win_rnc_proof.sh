#!/bin/bash
# Proof that the game's own relocated assembly runs on native Windows: symbolize the RNC
# decompressor, assemble it as 32-bit Windows COFF (i686-w64-mingw32-as), link with a C harness
# into a Windows .exe (i686-w64-mingw32-gcc). Run the .exe on Windows to verify it decompresses.
set -e
cd /work
gen=build/asmsym_rnc
mkdir -p "$gen"
[ -f build/obj1_full.bin ] || python3 tools/linearize.py >/dev/null

python3 - <<'PY'
import json, subprocess, sys, os
fns = {f["name"]: f for f in json.load(open("manifest/functions.json"))["functions"]}
for n in ["rnc_read_be_len", "rnc_input_bits", "rnc_read_huffman", "rnc_make_huffman", "rnc_decompress"]:
    f = fns[n]
    r = subprocess.run([sys.executable, "tools/asm_symbolize.py", "--underscore", n, f["addr"], str(f["size"])],
                       capture_output=True, text=True)
    s = r.stdout
    # strip the ELF-only GNU-stack note (harmless-but-noisy for COFF)
    s = "\n".join(l for l in s.splitlines() if "GNU-stack" not in l)
    open(os.path.join("build/asmsym_rnc", n + ".s"), "w").write(s)
print("symbolized 5 RNC routines")
PY

AS=i686-w64-mingw32-as
CC=i686-w64-mingw32-gcc
for s in rnc_read_be_len rnc_input_bits rnc_read_huffman rnc_make_huffman rnc_decompress; do
    "$AS" -o "$gen/$s.o" "$gen/$s.s"
done
# -ffixed-ebx: the game's asm follows the Watcom convention (eax/ebx/ecx/edx are caller-saved
# scratch), but the C ABI treats ebx as callee-saved. Reserving ebx in any TU that calls into
# the asm stops gcc keeping a live value there for the asm to clobber.
"$CC" -O2 -ffixed-ebx port/demo_asm_native.c "$gen"/*.o -o "$gen/rnc_win.exe"
echo "built $gen/rnc_win.exe"
file "$gen/rnc_win.exe" | cut -d: -f2-
