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
#include <stdio.h>

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

/* flic_load_palette -- portable C reimplementation (was asm writing the VGA DAC directly).
 * Rebuilds the 256-colour palette from the FLIC run-length palette script and hands it to the
 * platform. Script @ g_flic_pal_script (0x10ab0) = [u16 runs] then per run [u8 skip][u8 count]
 * [count*3 rgb]; skip advances the DAC cursor, count==0 means 256. Only acts when the player's
 * mode flag (0xbdf4) is 1; clears the palette-dirty flag (0xbe2e). */
void shim_flic_load_palette(void) {
    unsigned char *script = *(unsigned char **)(__dgroup + 0x10ab0);
    unsigned char *dac    = *(unsigned char **)(__dgroup + 0x10aa8);   /* 768-byte build buffer */
    unsigned char *cur;
    unsigned runs, r;
    *(unsigned short *)(__dgroup + 0xbe2e) = 0;               /* clear dirty flag */
    if (*(unsigned *)(__dgroup + 0xbdf4) != 1 || !script || !dac) return;
    runs = *(unsigned short *)script; script += 2;
    cur = dac;
    for (r = 0; r < runs; r++) {
        unsigned skip = *script++;
        unsigned count = *script++; if (count == 0) count = 256;
        cur += skip * 3;
        memcpy(cur, script, count * 3);
        script += count * 3; cur += count * 3;
    }
    plat_set_palette(dac);
}

#include <stdlib.h>
static int g_pres = 0;
void shim_vga_planar_present(void) {
    if (getenv("SYN_DEBUG") && g_pres++ < 3)
        fprintf(stderr, "[present #%d] g_screen_buf=%p\n", g_pres, (void *)G_SCREEN_BUF);
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
