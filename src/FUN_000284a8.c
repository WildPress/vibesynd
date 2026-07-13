/* FUN_000284a8 @ 0x284a8 - DOS/DPMI transfer-buffer request, opcode 0x94.
 * Copies (ushort)len bytes from near src into the global transfer buffer
 * (far ptr: off dword @0xdf2a, sel word @0xdf2e) via an inlined far memcpy
 * (push ds/es, rep movsd + movsb tail, Watcom F2 REPNE prefix). Then through
 * the caller-supplied request block sel:>off: word[6] = g_df28,
 * word[8] = (ushort)len, byte[0] = 0x94; submits the far ptr via 0x27d88
 * (8-byte far-ptr arg: push seg widened from GS + push off) returning short
 * (-1 => -0x63). Busy-waits on status byte [0x31] until != 0xff; non-zero
 * status reported via 0x289a8(g_376c, 0x249, status); returns -status as int.
 * The far copy is a hand pragma (identical bytes to the _fmemcpy intrinsic)
 * so its modify list leaves EBX free for `off`.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned char __far *g_df2afp;   /* transfer buffer far ptr: off @0xdf2a, sel @0xdf2e */
extern unsigned short g_df28;
extern char g_376c[];
extern short FUN_00027d88(unsigned char __far *p);
extern void FUN_000289a8(char *s, int b, int c);

extern void fmemcpy94(unsigned char __far *dst, unsigned char __far *src, unsigned n);
#pragma aux fmemcpy94 = "db 30" "db 6" "db 87" "db 145" "db 142" "db 216" "db 142" "db 194" "db 137" "db 200" "db 193" "db 233" "db 2" "db 242" "db 165" "db 138" "db 200" "db 128" "db 225" "db 3" "db 242" "db 164" "db 88" "db 7" "db 31" parm [dx edi] [cx esi] [eax] modify exact [eax ecx esi edi];

int FUN_000284a8(unsigned int off, unsigned short sel, void *src, int len)
{
    unsigned char __far *p;

    fmemcpy94(g_df2afp, (unsigned char __far *)src, (unsigned short)len);
    p = sel :> (unsigned char *)off;
    *(unsigned short __far *)(p + 6) = g_df28;
    *(unsigned short __far *)(p + 8) = len;
    p[0] = 0x94;
    if (FUN_00027d88(p) == -1)
        return -0x63;
    p = sel :> (unsigned char *)off;
    while (p[0x31] == 0xff)
        ;
    if (p[0x31] != 0)
        FUN_000289a8(g_376c, 0x249, p[0x31]);
    return -(int)((sel :> (unsigned char *)off)[0x31]);
}
