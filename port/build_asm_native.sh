#!/bin/bash
# build_asm_native.sh -- regenerate + build the "game's asm runs natively" proof.
#
# Symbolizes the RNC decompressor from the transcriptions (tools/asm_symbolize.py), assembles
# the relocatable .s, links them with port/demo_asm_native.c, and runs it on a real palette.
# Demonstrates the native-relocation pipeline end to end. Needs gcc-multilib (-m32) + binutils.
#
# NOTE: must be non-PIE (-no-pie) -- the asm uses absolute addressing into __dgroup.
set -e
cd "$(dirname "$0")/.."
gen=build/asmsym_rnc
mkdir -p "$gen"

# addr/size of each RNC routine from the manifest
python3 - "$gen" <<'PY'
import json, subprocess, sys, os
gen = sys.argv[1]
fns = {f["name"]: f for f in json.load(open("manifest/functions.json"))["functions"]}
for n in ["rnc_read_be_len", "rnc_input_bits", "rnc_read_huffman", "rnc_make_huffman", "rnc_decompress"]:
    f = fns[n]
    r = subprocess.run([sys.executable, "tools/asm_symbolize.py", n, f["addr"], str(f["size"])],
                       capture_output=True, text=True)
    open(os.path.join(gen, n + ".s"), "w").write(r.stdout)
print("symbolized 5 RNC routines ->", gen)
PY

for s in rnc_read_be_len rnc_input_bits rnc_read_huffman rnc_make_huffman rnc_decompress; do
    as --32 -o "$gen/$s.o" "$gen/$s.s"
done
# -ffixed-ebx: the game's asm is Watcom (ebx = caller-saved scratch), but the C ABI treats ebx
# as callee-saved; reserving it keeps gcc from parking a live value there across the asm call.
gcc -m32 -O2 -no-pie -fno-pie -ffixed-ebx -z noexecstack port/demo_asm_native.c "$gen"/*.o -o "$gen/demo_asm_native"
echo "built: $gen/demo_asm_native"
"$gen/demo_asm_native" "$@"
