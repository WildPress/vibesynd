/* netface.h -- the socket protocol between the 32-bit game (in WSL) and a NATIVE viewer process.
 *
 * A Windows .exe can't share Linux shared memory with the game, so the native-window viewer talks
 * to the game over a local TCP socket (WSL2 forwards localhost). The game is the server; the viewer
 * connects. Two fixed-size messages, sent whole:
 *   game  -> viewer : syn_frame_t  (the 8-bit frame + 6-bit palette)
 *   viewer -> game  : syn_input_t  (mouse + a batch of key events)
 * Fixed layout with explicit widths so 32-bit game and 64-bit/Windows viewer agree.
 */
#ifndef SYN_NETFACE_H
#define SYN_NETFACE_H

#include <stdint.h>

#define SYN_PORT 47800
#define SYN_W 320
#define SYN_H 200
#define SYN_NKEYS 32

typedef struct {
    uint32_t frame;
    uint8_t  fb[SYN_W * SYN_H];
    uint8_t  pal[768];
} syn_frame_t;

typedef struct {
    int32_t mx, my, buttons;
    int32_t nkeys;                 /* number of key events in this message */
    int32_t keys[SYN_NKEYS];       /* (scancode<<8)|ascii, BIOS-style */
} syn_input_t;

#endif
