#!/bin/bash
# build_loader.sh -- prove the game's own file loader runs natively: link the code blob (with
# the DOS file-I/O leaves redirected to POSIX shims) + the DGROUP data model + a harness that
# calls game_load_unpack_file on a real compressed file. Needs gcc-multilib + binutils.
set -e
cd "$(dirname "$0")/.."
b=port/gen/blob
mkdir -p "$b"
[ -f port/gen/dgroup.bin ] || python3 tools/port_data.py >/dev/null
python3 tools/asm_emit_blob.py --shims sopen,read,write,close,lseek,tell >/dev/null
as --32 -o "$b/game_blob.o" "$b/game_blob.s"
as --32 -o "$b/dgroup.o"    port/dgroup.s
gcc -m32 -no-pie -fno-pie -z noexecstack -O2 \
    port/demo_loader_native.c port/shims_file.c "$b/game_blob.o" "$b/dgroup.o" \
    -o "$b/demo_loader_native"
"$b/demo_loader_native" "$@"
