/* shims_video.c -- native (SDL2) replacements for the game's VGA routines.
 *
 * The game composites each frame into a linear 8-bit buffer pointed to by g_screen_buf
 * (DGROUP 0x5368) and calls vga_planar_present to push it to mode-X VGA. The shims route
 * that linear buffer straight to the SDL/GPU layer (port/platform_sdl.c) instead. Palette
 * uploads are captured for the 8-bit -> RGBA conversion.
 *
 * The game is -4s (stack calling), so these are plain cdecl. asm_emit_blob.py --shims
 * redirects each VGA routine's blob entry to shim_<name>.
 */
#include "platform.h"
#include "gfx.h"
#include <string.h>

extern unsigned char __dgroup[];
#define G_SCREEN_BUF (*(unsigned char **)(__dgroup + 0x5368))   /* front offscreen buffer ptr */

static int g_video_up = 0;

static void ensure_video(void) {
    if (!g_video_up) { plat_video_init(SYN_W, SYN_H); g_video_up = 1; }
}

void shim_set_video_mode(int mode) { (void)mode; ensure_video(); }

void shim_upload_palette(const unsigned char *pal) {
    if (pal) plat_set_palette(pal);      /* 256 * (r,g,b), 6-bit VGA */
}

void shim_vga_planar_present(void) {
    unsigned char *gsb = G_SCREEN_BUF;
    ensure_video();
    if (gsb) {
        unsigned char *fb = plat_framebuffer();
        if (gsb != fb) memcpy(fb, gsb, SYN_W * SYN_H);
        plat_present();
    }
}
void shim_present_frame_planar(void) { shim_vga_planar_present(); }
void shim_present_patch_gated(void)  { shim_vga_planar_present(); }

void shim_clear_vga_screen(void) {
    unsigned char *gsb = G_SCREEN_BUF;
    if (gsb) memset(gsb, 0, SYN_W * SYN_H);
}

/* software-cursor / sprite background save-restore and VGA register pokes: no-ops on SDL */
void shim_save_cursor_bg(void)    {}
void shim_restore_cursor_bg(void) {}
void shim_save_sprite_bg(void)    {}
void shim_restore_sprite_bg(void) {}
void shim_wait_vsync(void)        {}
void shim_reset_gfx_write_mode(void) {}
void shim_flic_load_palette(void) {}
