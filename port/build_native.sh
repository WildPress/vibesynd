#!/bin/bash
# build_native.sh -- link the whole game (code blob + DGROUP data model) into a native
# 32-bit executable. Proves code+data integrate with 0 undefined symbols.
#
# NOT yet runnable: the entry still points at the blob start (not the game's startup), and
# the ~64 hardware routines will fault until shimmed. This is the link milestone; the run
# milestone needs the entry wired + platform shims.
set -e
cd "$(dirname "$0")/.."
b=port/gen/blob
mkdir -p "$b"
[ -f port/gen/dgroup.bin ] || python3 tools/port_data.py >/dev/null
python3 tools/asm_emit_blob.py >/dev/null
as --32 -o "$b/game_blob.o" "$b/game_blob.s"
as --32 -o "$b/dgroup.o"     port/dgroup.s
cat > "$b/entry.s" <<'ASM'
    .text
    .globl _start
_start:
    call __code
    movl $1, %eax
    xorl %ebx, %ebx
    int  $0x80
    .section .note.GNU-stack,"",@progbits
ASM
as --32 -o "$b/entry.o" "$b/entry.s"
ld -m elf_i386 -no-pie -Ttext=0x400000 -e _start "$b/entry.o" "$b/game_blob.o" "$b/dgroup.o" -o "$b/game_native"
echo "LINKED $b/game_native  ($(nm -u "$b/game_native" | wc -l) undefined symbols)"
size "$b/game_native"
