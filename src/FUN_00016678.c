/* framed @ 0x16678.  TRUE extent 0x16678-0x16735 = 190B (0xbe); manifest size
   64 is UNDER-COUNTED (Ghidra bounds stop at 0x166b7, mid-body, before the RET
   at 0x16735).  Reads the current record g_e4a0[g_10b16] (record stride 0x417),
   divides its leading dword by 168 to get an index clamped to <=23, formats a
   string from (idx, x, y) via fn_3a4fa and draws it via fn_36698.

   NEAR-MISS 187/190 (masked), recipe -4s -oneatx -zp8 -s -zq.  Body is
   byte-faithful and all 8 relocations line up EXCEPT one instruction:
     target @0x7d:  68 70 00 00 00   PUSH 0x70   (imm32)
     ours   @0x7d:  6a 70            PUSH 0x70   (imm8, sign-extended)
   Watcom 9.5b unconditionally shrinks the 0x70 arg to a 2-byte imm8 push; the
   shipped binary keeps the 5-byte imm32 form (it still uses 6a for its other
   small pushes 0x0a/0x0e/0x10/-2/0, so this is a push-imm8 peephole-threshold
   difference between Watcom micro-versions, NOT source-reachable).  Confirmed
   unreachable: 8 source forms (long/unsigned/pointer casts, prototyped pointer
   param) x 7 recipes all emit 6a70.  This single 3-byte divergence also accounts
   for the entire 190-vs-187 length gap. */
extern short g_10b16;
extern unsigned char g_e4a0[];
extern int g_11be4;
extern void fn_3a4fa();
extern void fn_36698();

void FUN_00016678(void)
{
    char buf[40];
    unsigned short idx;

    idx = *(unsigned *)(g_e4a0 + (int)g_10b16 * 0x417) / 0xa8;
    if (idx == 0x18)
        idx = 0x17;
    fn_3a4fa(buf, 0x70, idx,
             *(unsigned short *)(g_e4a0 + (int)g_10b16 * 0x417 + 4),
             *(unsigned short *)(g_e4a0 + (int)g_10b16 * 0x417 + 6));
    fn_36698(buf, 0x1f4, 0xa, 0x10, 0x166, g_11be4, -2, 0xe, 0, 0);
}
