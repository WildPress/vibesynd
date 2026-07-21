/* platform_stub.c -- do-nothing platform backend, so the native build can LINK and run
 * (start + exit) before the SDL backend exists. Replaced by platform_sdl.c subsystem by
 * subsystem. Every function is a no-op / returns empty. */
#include "platform.h"
#include <stdlib.h>
#include <stdio.h>

static uint8_t g_fb[320 * 200];

void      plat_video_init(int w, int h)              { (void)w; (void)h; }
uint8_t  *plat_framebuffer(void)                     { return g_fb; }
void      plat_set_palette(const uint8_t *rgb768)    { (void)rgb768; }
void      plat_present(void)                         { }

int  plat_poll_key(void)                             { return 0; }
void plat_mouse(int *x, int *y, int *b)              { if (x)*x=0; if (y)*y=0; if (b)*b=0; }
int  plat_want_quit(void)                            { return 1; }   /* stub: exit immediately */

uint32_t plat_ticks_ms(void)                         { return 0; }
void     plat_sleep_ms(uint32_t ms)                  { (void)ms; }

void plat_audio_init(void)                           { }
void plat_play_sound(int id)                         { (void)id; }
void plat_play_music(int track)                      { (void)track; }

void *plat_fopen(const char *path, const char *mode) { return fopen(path, mode); }
long  plat_fread(void *buf, long sz, void *fh)       { return (long)fread(buf, 1, (size_t)sz, (FILE*)fh); }
void  plat_fclose(void *fh)                          { if (fh) fclose((FILE*)fh); }
