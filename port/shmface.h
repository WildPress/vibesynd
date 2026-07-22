/* shmface.h -- the shared-memory interface between the 32-bit game process and the 64-bit
 * SDL viewer. The game (which must stay 32-bit) writes its frame and palette here; the viewer
 * (64-bit, so it links the SDL that's already installed) presents them in a window and writes
 * input back. This sidesteps needing a 32-bit SDL build.
 *
 * Layout is fixed-size and identical under -m32 and -m64 (explicit widths, no pointers), so both
 * processes agree on the struct. Sync is a frame counter (viewer presents when it changes) plus a
 * simple key ring; tearing on the framebuffer is harmless for display.
 */
#ifndef SYN_SHMFACE_H
#define SYN_SHMFACE_H

#include <stdint.h>

#define SYN_SHM_NAME "/syndicate_port_shm"
#define SYN_SHM_MAGIC 0x53594e31u   /* "SYN1" */
#define SYN_W 320
#define SYN_H 200
#define SYN_KEYS 256

typedef struct {
    volatile uint32_t magic;
    volatile uint32_t frame;                 /* game increments on each present */
    volatile uint32_t quit;                  /* viewer sets on window close      */
    uint8_t  fb[SYN_W * SYN_H];              /* 8-bit game frame                 */
    uint8_t  pal[768];                       /* 6-bit VGA palette (r,g,b)         */
    volatile int32_t  mouse_x, mouse_y, mouse_buttons;
    volatile uint32_t key_head, key_tail;    /* viewer writes tail, game reads head */
    int32_t  keys[SYN_KEYS];                 /* (scancode<<8)|ascii, BIOS-style   */
} syn_shm_t;

#endif
