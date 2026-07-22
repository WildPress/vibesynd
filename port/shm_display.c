/* shm_display.c -- game-side platform backend that publishes frames to shared memory and reads
 * input back, for the 64-bit SDL viewer to present. Implements platform.h (a drop-in for
 * platform_headless.c) plus plat_present_buf. The game stays 32-bit and SDL-free.
 */
#include "platform.h"
#include "shmface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

void plat_present_buf(const unsigned char *buf);   /* defined below */

static syn_shm_t *g_shm;

static void shm_init(void) {
    int fd;
    if (g_shm) return;
    fd = shm_open(SYN_SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) { perror("shm_open"); return; }
    if (ftruncate(fd, sizeof(syn_shm_t)) != 0) perror("ftruncate");
    g_shm = mmap(NULL, sizeof(syn_shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (g_shm == MAP_FAILED) { g_shm = NULL; perror("mmap"); return; }
    g_shm->magic = SYN_SHM_MAGIC;
    g_shm->frame = 0; g_shm->quit = 0;
    g_shm->key_head = g_shm->key_tail = 0;
    fprintf(stderr, "[shm] published %s (%zu bytes)\n", SYN_SHM_NAME, sizeof(syn_shm_t));
}

void plat_video_init(int w, int h) { (void)w; (void)h; shm_init(); }

uint8_t *plat_framebuffer(void) { static uint8_t fb[SYN_W * SYN_H]; return fb; }

void plat_set_palette(const uint8_t *rgb768) { if (g_shm) memcpy((void *)g_shm->pal, rgb768, 768); }

void plat_present(void) { plat_present_buf(plat_framebuffer()); }

void plat_present_buf(const unsigned char *buf) {
    if (!g_shm) return;
    memcpy((void *)g_shm->fb, buf, SYN_W * SYN_H);
    g_shm->frame++;
}

/* --- input, fed by the viewer via the shm --- */
int plat_poll_key(void) {
    uint32_t h;
    if (!g_shm) return 0;
    h = g_shm->key_head;
    if (h == g_shm->key_tail) return 0;
    int k = g_shm->keys[h % SYN_KEYS];
    g_shm->key_head = h + 1;
    return k;
}
void plat_mouse(int *x, int *y, int *b) {
    if (g_shm) { if (x) *x = g_shm->mouse_x; if (y) *y = g_shm->mouse_y; if (b) *b = g_shm->mouse_buttons; }
    else { if (x) *x = 0; if (y) *y = 0; if (b) *b = 0; }
}
int plat_want_quit(void) { return g_shm ? (int)g_shm->quit : 0; }

uint32_t plat_ticks_ms(void) { return 0; }
void plat_sleep_ms(uint32_t ms) { (void)ms; }
void plat_audio_init(void) {}
void plat_play_sound(int id) { (void)id; }
void plat_play_music(int t) { (void)t; }
void *plat_fopen(const char *p, const char *m) { return fopen(p, m); }
long plat_fread(void *b, long s, void *fh) { return (long)fread(b, 1, (size_t)s, (FILE *)fh); }
void plat_fclose(void *fh) { if (fh) fclose((FILE *)fh); }
