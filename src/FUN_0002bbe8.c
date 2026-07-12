/* NEAR-MISS @ 0x2bbe8 -- 160 vs 159 code bytes; JUMP TABLE FULLY RECOVERED via
 * tools/lefix.py (the payoff of the fixed parser). 5-entry dispatcher at
 * obj1:+0x1e480: word +0 -> case 1/2/3 add 1/2/3, cases 0/4 + default add 0.
 * (Manifest size was undercounted 140; corrected to 159 = through the ret.)
 *
 * Animation-frame ticker over an 18-byte-record list at p, ending at a record
 * whose word +0 == -99 (0xff9d). Per active record (delay byte +0xa != 0):
 * decrement delay, add the per-type step to base frame 0xd4+word[+0x10]; if the
 * frame changed, look up sprite g_5348[frame] into word +0xc and store frame to
 * +0xe; draw via 0x1b798(sprite, x=+2, y=+4).
 *
 * Structure, the dense-0..4 jump table, the entry-guard do-while loop, the ESI
 * old-frame hold, and the g_5348 pointer-materialise ALL match. PARKED on the
 * back-half register tie-break: the target builds base in EDX and accumulates
 * the frame in EAX (mov edx,0xd4; add eax,edx; sprite base ECX, result DX);
 * every C form here rotates that to ECX/CX. Inlining base/old flips ESI->EDI
 * instead. Same class as 0x2d5b8 / 0x28ec8. Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned short *g_5348;
extern void FUN_0001b798(unsigned short spr, short x, short y);

void FUN_0002bbe8(unsigned char *p)
{
    if (*(short *)p != -99) {
        do {
            if (p[0xa] != 0) {
                unsigned short old = *(unsigned short *)(p + 0xe);
                unsigned short base;
                unsigned short a;
                int sw = 0;

                p[0xa]--;
                switch (*(unsigned short *)p) {
                case 0: break;
                case 1: sw = 1; break;
                case 2: sw = 2; break;
                case 3: sw = 3; break;
                case 4: break;
                }
                base = (unsigned short)(0xd4 + *(unsigned short *)(p + 0x10));
                a = (unsigned short)(sw + base);
                if (a != old) {
                    *(unsigned short *)(p + 0xc) = g_5348[a];
                    *(unsigned short *)(p + 0xe) = a;
                }
                FUN_0001b798(*(unsigned short *)(p + 0xc),
                             *(short *)(p + 2), *(short *)(p + 4));
            }
            p += 0x12;
        } while (*(short *)p != -99);
    }
}
