#!/bin/bash
# build_video.sh -- drive the game's OWN present path to a frame, natively.
# Links the code blob (file+video+memory leaves shimmed) + data model + the video shims
# (SDL-bound gfx) + a headless backend that dumps the presented frame to a PPM. Proves the
# render shim path pixel-for-pixel without a 32-bit SDL build. Needs gcc-multilib + binutils.
set -e
cd "$(dirname "$0")/.."
b=port/gen/blob
mkdir -p "$b"
[ -f port/gen/dgroup.bin ] || python3 tools/port_data.py >/dev/null
SHIMS="sopen,read,write,close,lseek,tell,set_video_mode,upload_palette,vga_planar_present,present_frame_planar,present_patch_gated,clear_vga_screen,save_cursor_bg,restore_cursor_bg,save_sprite_bg,restore_sprite_bg,wait_vsync,reset_gfx_write_mode,flic_load_palette,os_getmem,dpmi_dos_alloc_paras,brk,heap_grow"
python3 tools/asm_emit_blob.py --shims "$SHIMS" >/dev/null
as --32 -o "$b/game_blob.o" "$b/game_blob.s"
as --32 -o "$b/dgroup.o"    port/dgroup.s
gcc -m32 -no-pie -fno-pie -z noexecstack -O2 \
    port/demo_video_native.c port/shims_file.c port/shims_video.c port/shims_mem.c \
    port/gfx_soft.c port/platform_headless.c "$b/game_blob.o" "$b/dgroup.o" \
    -o "$b/demo_video_native"
"$b/demo_video_native" "$@"
