/* platform.h -- the boundary the game logic calls and the platform provides.
 *
 * The DOS build (branch `main`) satisfies these with the src/lib assembly layer
 * (VGA/PIT/DOS int21h). The native build (this branch) satisfies them with an SDL2 +
 * GPU implementation in port/platform_sdl.c. The game logic includes this header and is
 * unaware which backend is linked.
 *
 * This is the initial interface skeleton -- the exact set grows as each src/lib routine
 * the game logic calls is pulled behind it. Signatures here mirror the decompiled ones so
 * the same call sites work against either backend.
 */
#ifndef SYND_PLATFORM_H
#define SYND_PLATFORM_H

#include <stdint.h>

/* ---- video ---------------------------------------------------------------
 * The game renders into an 8-bit palettized framebuffer. The platform uploads it to the
 * GPU as a texture and presents it (resizable window, scaling, vsync). */
void      plat_video_init(int width, int height);      /* create window + GPU surface */
uint8_t  *plat_framebuffer(void);                       /* the 8-bit game framebuffer   */
void      plat_set_palette(const uint8_t *rgb768);       /* 256 * (r,g,b) 0..63 (VGA DAC) */
void      plat_present(void);                            /* upload framebuffer, present  */

/* ---- input ---------------------------------------------------------------
 * Replaces the INT9 keyboard / INT33 mouse handlers. */
int  plat_poll_key(void);            /* next key scancode, 0 if none */
void plat_mouse(int *x, int *y, int *buttons);
int  plat_want_quit(void);           /* window close requested */

/* ---- timing --------------------------------------------------------------
 * Replaces the INT8 PIT tick the game busy-waits (g_timer_tick vs g_game_speed). */
uint32_t plat_ticks_ms(void);
void     plat_sleep_ms(uint32_t ms);

/* ---- audio ---------------------------------------------------------------
 * Replaces the AdLib/SoundBlaster/MIDI driver. Stubbed first (silent playable). */
void plat_audio_init(void);
void plat_play_sound(int id);
void plat_play_music(int track);

/* ---- file I/O ------------------------------------------------------------
 * Replaces int21h DOS file calls; a thin wrapper over stdio, reading the user's
 * data/ directory. */
void  *plat_fopen(const char *path, const char *mode);
long   plat_fread(void *buf, long size, void *fh);
void   plat_fclose(void *fh);

#endif /* SYND_PLATFORM_H */
