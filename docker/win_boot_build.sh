#!/bin/bash
# win_boot_build.sh -- build the whole game as a native Windows console .exe that boots via its
# own game_startup_main and renders a frame headlessly (PPM), driven by the Windows VEH DOS-int
# emulator (dosint_win.c). The COFF counterpart of port/build_boot.sh. Run in the container; run
# the resulting build/win/demo_boot_win.exe on Windows from a dir containing a lowercase data/.
set -e
cd /work
b=port/gen/blob
out=build/win
mkdir -p "$b" "$out"
[ -f build/obj1_full.bin ] || python3 tools/linearize.py >/dev/null

# same shim surface as the Linux boot: file/video/memory/timer/input leaves + DOS helpers
SHIMS="sopen,read,write,close,lseek,tell,set_video_mode,vga_planar_present,present_frame_planar,present_patch_gated,clear_vga_screen,save_cursor_bg,restore_cursor_bg,save_sprite_bg,restore_sprite_bg,wait_vsync,reset_gfx_write_mode,upload_palette,flic_load_palette,outp,d_getvec,d_setvec,dos_exec,spawnve,os_getmem,dpmi_dos_alloc_paras,brk,heap_grow,malloc,nmalloc,free,nfree,int386,poll_key,frame_throttle,install_timer_isr,uninstall_timer_isr,reprogram_pit_ch0"
python3 tools/asm_emit_blob.py --underscore --shims "$SHIMS" >/dev/null
python3 tools/port_dgroup.py  --underscore >/dev/null

AS=i686-w64-mingw32-as
CC=i686-w64-mingw32-gcc
"$AS" -o "$b/game_blob.o"    "$b/game_blob.s"
"$AS" -o "$b/dgroup_fixed.o" "$b/dgroup_fixed.s"

# -ffixed-ebx: the game's asm is Watcom (ebx = caller-saved scratch); the C ABI keeps ebx
# callee-saved. Reserving it stops gcc parking a live value there across a call into the asm.
"$CC" -O2 -ffixed-ebx -o "$out/demo_boot_win.exe" \
    port/demo_boot_native.c port/shims_file.c port/shims_video.c port/shims_mem.c \
    port/shims_sys.c port/shims_dos.c port/dosint_win.c port/dosint_core.c \
    port/gfx_soft.c port/platform_headless.c \
    "$b/game_blob.o" "$b/dgroup_fixed.o"
echo "built $out/demo_boot_win.exe"
