#!/bin/bash
# build_boot.sh -- boot the game natively via its own startup_main, and render its title screen.
#
# Links the code blob (file/video/memory/timer/input leaves shimmed) + the relocated DGROUP data
# model + all shims + the DOS-int/port-IO/DAC trap emulator (port/dosint.c). Runs the game's real
# startup from a lowercase data/ dir of the user's own files:
#   startup_main -> CLIB init -> load data -> (drivers stubbed) -> intro FLIC -> TITLE SCREEN.
# A display thread snapshots the frame (g_screen_buf + captured VGA-DAC palette) to frame.ppm.
#
# SYN_NODLL=1 fails the gamedg/gamefm.dll MZ driver overlays so the game degrades to its own
# render path (the low-level VGA is shimmed / DAC-captured) instead of executing the overlays.
set -e
cd "$(dirname "$0")/.."
b=port/gen/blob
run=build/rundir
GOG="${SYN_DATA:-/mnt/c/Program Files (x86)/GOG Galaxy/Games/Syndicate Plus/SYNDICAT/DATA}"
mkdir -p "$b" "$run/data"
if [ ! -e "$run/data/hpalette.dat" ]; then
  for f in "$GOG"/*; do ln -sf "$f" "$run/data/$(basename "$f" | tr 'A-Z' 'a-z')"; done
fi
python3 tools/port_dgroup.py >/dev/null
# upload_palette / flic_load_palette are NOT shimmed: they write the VGA DAC, which the trap
# emulator captures as the palette. Present goes through the display thread, not these.
SHIMS="sopen,read,write,close,lseek,tell,set_video_mode,vga_planar_present,present_frame_planar,present_patch_gated,clear_vga_screen,save_cursor_bg,restore_cursor_bg,save_sprite_bg,restore_sprite_bg,wait_vsync,reset_gfx_write_mode,os_getmem,dpmi_dos_alloc_paras,brk,heap_grow,malloc,nmalloc,free,nfree,int386,poll_key,frame_throttle,install_timer_isr,uninstall_timer_isr,reprogram_pit_ch0"
python3 tools/asm_emit_blob.py --shims "$SHIMS" >/dev/null
as --32 -o "$b/game_blob.o" "$b/game_blob.s"
as --32 -o "$b/dgroup.o"    port/gen/blob/dgroup_fixed.s
gcc -m32 -no-pie -fno-pie -z noexecstack -O2 \
    port/demo_boot_native.c port/shims_file.c port/shims_video.c port/shims_mem.c \
    port/shims_sys.c port/dosint.c port/gfx_soft.c port/platform_headless.c \
    "$b/game_blob.o" "$b/dgroup.o" -lpthread -o "$b/demo_boot_native"
cd "$run" && SYN_NODLL=1 SYN_FRAME_OUT=frame.ppm timeout "${SYN_TIMEOUT:-12}" ../../port/gen/blob/demo_boot_native
