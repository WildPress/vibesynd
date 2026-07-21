/* gfx.h -- portable software render layer for the native port.
 *
 * The game composites every frame into a linear 8-bit offscreen buffer (g_screen_buf);
 * only the original vga_planar_present pushes it through mode-X planes to the VGA card.
 * The port keeps the linear buffer and replaces the present step: convert 8-bit indices
 * through the palette to RGBA and upload as a GPU texture (platform_sdl.c).
 *
 * These are the blitter primitives reimplemented in portable C, over that linear buffer,
 * standing in for the hand-asm plot_point / fill_bytes / blit_block (see docs/blitter.md).
 */
#ifndef PORT_GFX_H
#define PORT_GFX_H

#include <stdint.h>

#define SYN_W 320
#define SYN_H 200

/* Convert a 320x200 8-bit indexed frame + a 768-byte 6-bit VGA palette (values 0..63)
 * into a width*height RGBA8888 buffer (0xAABBGGRR little-endian, alpha 0xff). */
void gfx_indexed_to_rgba(const uint8_t *indices, int w, int h,
                         const uint8_t pal_6bit[768], uint32_t *out_rgba);

/* Blitter primitives over a linear 8-bit buffer of stride `w`. */
void gfx_clear(uint8_t *fb, int w, int h, uint8_t color);
void gfx_plot(uint8_t *fb, int w, int h, int x, int y, uint8_t color);
void gfx_fill_rect(uint8_t *fb, int w, int h, int x, int y, int rw, int rh, uint8_t color);
/* Copy a src rectangle into fb at (dx,dy); if `key >= 0`, pixels equal to key are skipped. */
void gfx_blit(uint8_t *fb, int w, int h, const uint8_t *src, int sw, int sh,
              int dx, int dy, int key);

#endif
