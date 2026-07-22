#!/bin/bash
# build_window.sh -- build the interactive native port as two processes:
#   * a 32-bit game process (the relocated blob + shims + DOS emulator), SDL-free, that publishes
#     its frame to shared memory and reads input back (port/shm_display.c);
#   * a 64-bit SDL viewer (port/viewer_sdl.c) that presents the frame in a window and feeds input.
# The split lets the viewer use the SDL that's already installed, with no 32-bit SDL needed.
#
# Build only. To run, use port/run_window.sh (launches both; needs a display, e.g. WSLg).
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
SHIMS="sopen,read,write,close,lseek,tell,set_video_mode,vga_planar_present,present_frame_planar,present_patch_gated,clear_vga_screen,save_cursor_bg,restore_cursor_bg,save_sprite_bg,restore_sprite_bg,wait_vsync,reset_gfx_write_mode,os_getmem,dpmi_dos_alloc_paras,brk,heap_grow,malloc,nmalloc,free,nfree,int386,poll_key,frame_throttle,install_timer_isr,uninstall_timer_isr,reprogram_pit_ch0"
python3 tools/asm_emit_blob.py --shims "$SHIMS" >/dev/null
as --32 -o "$b/game_blob.o" "$b/game_blob.s"
as --32 -o "$b/dgroup.o"    port/gen/blob/dgroup_fixed.s

# 32-bit game process (SDL-free; shm backend)
gcc -m32 -no-pie -fno-pie -z noexecstack -O2 \
    port/demo_boot_native.c port/shims_file.c port/shims_video.c port/shims_mem.c \
    port/shims_sys.c port/dosint.c port/shm_display.c \
    "$b/game_blob.o" "$b/dgroup.o" -lpthread -lrt -o "$b/game_boot"
echo "built $b/game_boot (32-bit game)"

# 64-bit SDL viewer
if pkg-config --exists sdl2 2>/dev/null; then SINC=$(pkg-config --cflags sdl2); SLIB=$(pkg-config --libs sdl2)
else SINC="-Ibuild/sdl2/include -Ibuild/sdl2/include/SDL2"; SLIB="$(ls /usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0) -lm"; fi
# shellcheck disable=SC2086
gcc -O2 -Wall $SINC port/viewer_sdl.c port/gfx_soft.c $SLIB -lrt -o "$b/viewer"
echo "built $b/viewer (64-bit SDL window)"
