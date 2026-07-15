/* frameless @ 0x28558: NetBIOS receive via the DPMI mailbox (opcode 0x95),
   sibling of matched 0x284a8 (0x94 send). Builds the NCB at sel:>off: word+6 =
   g_df1c, word+8 = g_df38, byte 0 = 0x95; submits via FUN_27d88 (-1 -> -0x63).
   async != 0 -> return 0 immediately. Else busy-wait status [0x31] != 0xff,
   copy the answer (length = NCB word+8) from the transfer buffer far ptr
   g_df1e (off@0xdf1e, sel@0xdf22) to near dst via an inlined far memcpy
   (db-transcribed pragma, same body as 0x284a8's fmemcpy94), then
   report+return -status on error else return the answer length word. */
extern unsigned short g_df1c;
extern unsigned short g_df38;
extern unsigned char __far *g_df1e;
extern char g_376c[];
extern short submit_ncb(unsigned char __far *p);
extern void FUN_000289a8(char *s, int b, int c);

extern void fmemcpy95(unsigned char __far *dst, unsigned char __far *src, unsigned n);
#pragma aux fmemcpy95 = "db 30" "db 6" "db 87" "db 145" "db 142" "db 216" "db 142" "db 194" "db 137" "db 200" "db 193" "db 233" "db 2" "db 242" "db 165" "db 138" "db 200" "db 128" "db 225" "db 3" "db 242" "db 164" "db 88" "db 7" "db 31" parm [dx edi] [cx esi] [eax] modify exact [eax ecx esi edi];

short netbios_recv95(unsigned int off, unsigned short sel, void *dst, unsigned short async)
{
    *(unsigned short __far *)((sel :> (unsigned char *)off) + 6) = g_df1c;
    *(unsigned short __far *)((sel :> (unsigned char *)off) + 8) = g_df38;
    (sel :> (unsigned char *)off)[0] = 0x95;
    if (submit_ncb(sel :> (unsigned char *)off) == -1)
        return -0x63;
    if (async != 0)
        return 0;
    while ((sel :> (unsigned char *)off)[0x31] == 0xff)
        ;
    fmemcpy95((unsigned char __far *)dst, g_df1e,
              *(unsigned short __far *)((sel :> (unsigned char *)off) + 8));
    if ((sel :> (unsigned char *)off)[0x31] != 0) {
        FUN_000289a8(g_376c, 0x260, (sel :> (unsigned char *)off)[0x31]);
        return -(sel :> (unsigned char *)off)[0x31];
    }
    return *(unsigned short __far *)((sel :> (unsigned char *)off) + 8);
}
