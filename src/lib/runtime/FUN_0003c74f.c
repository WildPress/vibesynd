/* C runtime: heap-top free-size probe @ 0x3c74f (CLIB3S, Watcom 9.5). Frameless leaf.
   If the heap-enabled flag ds:0xbfc4 is set and the last arena block (ds:0xbfec) ends
   exactly at the current break ds:0xc2ac (base + size + 8), returns that block's size
   word [block]; otherwise returns 0. Hand-asm db-transcription: absolute global refs are
   literal (they equal the resolved linear.bin addresses, no fixup on our side); the
   mid-function RET is kept, the final RET is supplied by the frameless wrapper. */
extern int __heaptop(void);
#pragma aux __heaptop = "db 131" "db 61" "db 196" "db 191" "db 0" "db 0" "db 0" "db 116" "db 23" "db 161" "db 236" "db 191" "db 0" "db 0" "db 139" "db 16" "db 1" "db 194" "db 131" "db 194" "db 8" "db 59" "db 21" "db 172" "db 194" "db 0" "db 0" "db 117" "db 3" "db 139" "db 0" "db 195" "db 49" "db 192" value [eax] modify exact [eax edx];
#pragma aux FUN_0003c74f modify [eax edx];
int FUN_0003c74f(void)
{
    return __heaptop();
}
