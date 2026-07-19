/* mouse_init_int33 @ 0x28b88 - mouse driver init via INT 33h.
 * set interrupt rate (ax=0x1c, bx=2), segread, reset/detect (ax=0),
 * if out.ax == 0xffff: install event handler 0x1b290 (ax=0xc, cx=0x1f,
 * edx=FP_OFF(handler), sregs.es=FP_SEG(handler)) via int386x, set rate
 * again (ax=0x1c, bx=1), malloc a cursor-save buffer sized by g_105
 * (1 -> 0x1100, 2 -> 0x14b0, else 0x1090) into g_df3c, on success
 * hide cursor (ax=2) and return 1; on malloc fail report via 0x3ad66
 * and return 0. Returns 0 if no mouse driver.
 * MATCHED (317/317, reloc-aware). Levers that mattered:
 * - i86.h FP_SEG pragma (parm caller [eax dx] value [dx]) transcribed inline:
 *   the far cast of the handler materialises the dead mov eax,<off> + mov dx,cs.
 * - Locals decl order sr/out/in puts out at esp+0, in at +0x1c, sregs at +0x38.
 * - in.ax=0xc written FIRST in the install block: 0xc homes in EDI at block
 *   top and its store sinks below the far-cast; writing it last gave an
 *   immediate word store instead.
 * - volatile-alias extern g_df3cv for the post-store null test reproduces
 *   cmp dword [0xdf3c],0 (plain name CSEs to test eax,eax).
 * - malloc called INSIDE each switch case (Watcom tail-merges call+store,
 *   leaving per-case push imm32); a size temp + single call costs +1B (50).
 * Recipe: -4s -oneatx -zp8 -s -zq */
extern unsigned char g_105;
extern void *g_df3c;
extern void * volatile g_df3cv;   /* volatile alias, same global: forces the cmp [mem],0 re-read */
extern char g_37fc[];
extern void int386(int inum, void *inr, void *outr);   /* int386 */
extern void segread(void *sregs);                       /* segread */
extern void FUN_0003b3e6(int inum, void *inr, void *outr, void *sregs); /* int386x */
extern void *malloc(unsigned n);                       /* malloc */
extern void printf(char *s);                           /* error */
extern void FUN_0001b290(void);                              /* mouse event handler */

extern unsigned short FP_SEG(void __far *p);
#pragma aux FP_SEG = parm caller [eax dx] value [dx];

int mouse_init_int33(void)
{
    int sr[3];
    int out[7];
    int in[7];

    *(unsigned short *)&in[0] = 0x1c;
    *(unsigned short *)&in[1] = 2;
    int386(0x33, in, out);
    segread(sr);
    *(unsigned short *)&in[0] = 0;
    int386(0x33, in, out);
    if (*(unsigned short *)&out[0] == 0xffff) {
        *(unsigned short *)&in[0] = 0xc;
        *(unsigned short *)&in[2] = 0x1f;
        in[3] = (int)FUN_0001b290;
        *(unsigned short *)&sr[0] = FP_SEG((void __far *)FUN_0001b290);
        FUN_0003b3e6(0x33, in, out, sr);
        *(unsigned short *)&in[0] = 0x1c;
        *(unsigned short *)&in[1] = 1;
        FUN_0003b3e6(0x33, in, out, sr);
        switch (g_105) {
        case 1:
            g_df3c = malloc(0x1100);
            break;
        case 2:
            g_df3c = malloc(0x14b0);
            break;
        default:
            g_df3c = malloc(0x1090);
            break;
        }
        if (g_df3cv != 0) {
            *(unsigned short *)&in[0] = 2;
            int386(0x33, in, out);
            return 1;
        }
        printf(g_37fc);
    }
    return 0;
}
