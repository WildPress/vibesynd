#!/bin/bash
# win_window_build.sh -- build the game as a native Windows .exe with a live SDL2 window (one
# process, no viewer split). Like win_boot_build.sh but links the in-process SDL backend
# (sdl_display.c) + 32-bit MinGW SDL2 instead of the headless PPM writer. Produces
# build/win/syndicate_win.exe (+ SDL2.dll beside it). Run it on Windows from a dir with a
# lowercase data/.
set -e
cd /work
b=port/gen/blob
out=build/win
SDL="${SYN_SDL_MINGW32:-/opt/sdl2-mingw32}"
mkdir -p "$b" "$out"
[ -f build/obj1_full.bin ] || python3 tools/linearize.py >/dev/null
[ -d "$SDL/include" ] || { echo "no 32-bit MinGW SDL2 at $SDL (set SYN_SDL_MINGW32)"; exit 1; }

SHIMS="sopen,read,write,close,lseek,tell,set_video_mode,vga_planar_present,present_frame_planar,present_patch_gated,clear_vga_screen,save_cursor_bg,restore_cursor_bg,save_sprite_bg,restore_sprite_bg,wait_vsync,reset_gfx_write_mode,upload_palette,flic_load_palette,outp,d_getvec,d_setvec,dos_exec,spawnve,os_getmem,dpmi_dos_alloc_paras,brk,heap_grow,malloc,nmalloc,free,nfree,int386,poll_key,frame_throttle,install_timer_isr,uninstall_timer_isr,reprogram_pit_ch0"
python3 tools/asm_emit_blob.py --underscore --shims "$SHIMS" >/dev/null
python3 tools/port_dgroup.py  --underscore >/dev/null

AS=i686-w64-mingw32-as
CC=i686-w64-mingw32-gcc
"$AS" -o "$b/game_blob.o"    "$b/game_blob.s"
"$AS" -o "$b/dgroup_fixed.o" "$b/dgroup_fixed.s"

# -ffixed-ebx: reserve ebx (Watcom asm treats it as scratch; the C ABI keeps it callee-saved).
"$CC" -O2 -ffixed-ebx -I"$SDL/include" -I"$SDL/include/SDL2" \
    -o "$out/syndicate_win.exe" \
    port/demo_boot_native.c port/shims_file.c port/shims_video.c port/shims_mem.c \
    port/shims_sys.c port/shims_dos.c port/dosint_win.c port/dosint_core.c \
    port/gfx_soft.c port/sdl_display.c \
    "$b/game_blob.o" "$b/dgroup_fixed.o" \
    -L"$SDL/lib" -lSDL2 -lwinmm -limm32 -lsetupapi -lversion -lole32 -loleaut32 -lgdi32
cp -f "$SDL/bin/SDL2.dll" "$out/SDL2.dll"
echo "built $out/syndicate_win.exe (+ SDL2.dll)"
