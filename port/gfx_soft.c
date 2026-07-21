/* gfx_soft.c -- portable software render layer (see gfx.h). */
#include "gfx.h"

void gfx_indexed_to_rgba(const uint8_t *indices, int w, int h,
                         const uint8_t pal_6bit[768], uint32_t *out_rgba) {
    /* expand each 6-bit VGA component (0..63) to 8-bit: c*255/63, precomputed */
    uint8_t lut[64];
    int i;
    for (i = 0; i < 64; i++) lut[i] = (uint8_t)((i * 255 + 31) / 63);
    for (i = 0; i < w * h; i++) {
        const uint8_t *p = &pal_6bit[indices[i] * 3];
        uint8_t r = lut[p[0] & 63], g = lut[p[1] & 63], b = lut[p[2] & 63];
        out_rgba[i] = 0xff000000u | ((uint32_t)b << 16) | ((uint32_t)g << 8) | r;
    }
}

void gfx_clear(uint8_t *fb, int w, int h, uint8_t color) {
    int i, n = w * h;
    for (i = 0; i < n; i++) fb[i] = color;
}

void gfx_plot(uint8_t *fb, int w, int h, int x, int y, uint8_t color) {
    if (x >= 0 && y >= 0 && x < w && y < h) fb[y * w + x] = color;
}

void gfx_fill_rect(uint8_t *fb, int w, int h, int x, int y, int rw, int rh, uint8_t color) {
    int yy, xx;
    for (yy = y; yy < y + rh; yy++) {
        if (yy < 0 || yy >= h) continue;
        for (xx = x; xx < x + rw; xx++) {
            if (xx < 0 || xx >= w) continue;
            fb[yy * w + xx] = color;
        }
    }
}

void gfx_blit(uint8_t *fb, int w, int h, const uint8_t *src, int sw, int sh,
              int dx, int dy, int key) {
    int sy, sx;
    for (sy = 0; sy < sh; sy++) {
        int y = dy + sy;
        if (y < 0 || y >= h) continue;
        for (sx = 0; sx < sw; sx++) {
            int x = dx + sx;
            uint8_t px;
            if (x < 0 || x >= w) continue;
            px = src[sy * sw + sx];
            if (key >= 0 && px == (uint8_t)key) continue;
            fb[y * w + x] = px;
        }
    }
}
