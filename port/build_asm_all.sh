#!/bin/bash
# build_asm_all.sh -- emit ALL carved game functions as relocatable asm, assemble them, and
# merge into one native object, reporting the remaining undefined surface.
#
# Proves how much of the game links natively from the transcriptions. Result today: all 555
# carved functions (those with bytes in OBJECT1.linear) assemble and cross-resolve into one
# ~248 KB object; what's left undefined is the data model (__obj bases), the 13 cut-off prefix
# functions, and ~75 uncarved callees. Needs binutils (as/ld/nm, 32-bit).
set -e
cd "$(dirname "$0")/.."
gen=port/gen/asm
obj=build/asm_obj
mkdir -p "$obj"

python3 tools/asm_emit_all.py "$gen" | sed 's/^/  /'

rm -f "$obj"/*.o
n=0
for s in "$gen"/*.s; do
    as --32 -o "$obj/$(basename "$s" .s).o" "$s"
    n=$((n + 1))
done
echo "assembled $n objects"
ld -r -m elf_i386 "$obj"/*.o -o "$obj/game_all.o"
echo "merged -> $obj/game_all.o ($(stat -c%s "$obj/game_all.o") bytes)"
nm -u "$obj/game_all.o" | awk '{print $2}' | sort > "$obj/_undef.txt"
echo "undefined after merge: $(wc -l < "$obj/_undef.txt")  (__obj: $(grep -c '^__obj' "$obj/_undef.txt"), fn_: $(grep -c '^fn_' "$obj/_undef.txt"), named: $(grep -vcE '^(__obj|fn_)' "$obj/_undef.txt"))"
