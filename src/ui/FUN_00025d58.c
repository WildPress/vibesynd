/* @ 0x25d58: PARKED near-miss (prologue reg-save-order wall, GAME region).
   Body is byte-faithful (all 6+ calls, constants, globals and the whole
   instruction sequence reproduce the target under -4s -oneatx) BUT the target
   uses a regs-first EBP FRAME: `push ebx;push esi;push edi;push ebp;mov ebp,esp;
   sub esp,0x28`, keeping param1 MEMORY-HOMED at [ebp+0x14] (re-read into bl/dl
   at all 4 use sites, which straddle esp-in-flux nested-call regions). Our
   Watcom 9.5b instead tracks the esp deltas through each cdecl cleanup and
   addresses everything esp-relative -> it OMITS the frame and PROMOTES param1
   into ESI, cascading a param1 EBX<->ESI register-role swap + esp-vs-ebp
   addressing through the whole body. First diff at 0x4 (missing `89 e5`).
   Recipes surveyed: -oneatx/-oneat/-ot/-oi/-ox -> frameless (no 89 e5);
   -of -> frame-FIRST (`55 89 e5 53 56 57`, wrong order); -os -> ENTER-frame,
   drops edi; -od -> EXACT regs-first manual frame `53 56 57 55 89 e5` (first
   diff only at 0x6: sub 0x30/imm32 vs 0x28/imm8) but de-optimizes the body.
   volatile param + inline/library alloca do not induce the optimized frame.
   Same wall class as the 0x3a000 runtime-region prologue reg-save-order wall
   (playbook §3), here proving our 9.5b vs the original build differ on
   frame-omission for esp-in-flux, memory-homed-param functions. TRUE SIZE
   1234B (matches manifest). Semantics below are believed correct.

   mission/agent detail panel drawer. param1 = record index into the parallel
   10-byte g_539c table and 19-byte g_b072 table. Centres a title (width via
   0x36648, centred x = 0xb8 + (0x118-w)/2), draws several localized label/value
   rows via the 0x36698 text drawer, then two owner/claim-gated sections.
   Strings come from per-language pointer tables (index g_a50d*4); the title
   table g_41a4 is 2D (row = g_b072[id]*12). */
extern unsigned char *g_11be4;
extern unsigned char g_a50d;
extern unsigned char g_b072[];
extern unsigned char g_539c[];
extern unsigned char g_e49c[];
extern unsigned short g_10b16;
extern unsigned short g_5390;
extern unsigned short g_5392;
extern char *g_41a4[];
extern char *g_469c[];
extern char *g_46b4[];
extern char *g_46a8[];
extern char *g_46cc[];
extern char *g_46c0[];
extern char *g_45d0[];
extern char *g_46d8[];
extern char *g_46e4[];
extern char *g_46f0[];
extern char *g_46fc[];
extern char *g_4708[];
extern char *g_4714[];
extern char g_365c[];
extern char g_3660[];

extern unsigned short FUN_00036648(char *s, unsigned char *tbl, unsigned short base,
                                   signed char adj, int a5);
extern void FUN_00036698(char *s, unsigned short x, int a3, int a4, unsigned short a5,
                         unsigned char *tbl, signed char a7, signed char a8, int a9,
                         int a10);
extern void FUN_00036208(char *a, unsigned short b, unsigned short c, unsigned short d,
                         unsigned short e, signed char f, unsigned char g);
extern void FUN_0003a4fa(char *buf, char *fmt, ...);
extern void FUN_000265d8(unsigned char p);
extern char FUN_000264a8(unsigned char p);

void FUN_00025d58(unsigned char param1)
{
    char buf[40];
    unsigned short colour;
    short claim;

    /* section 1: centred title */
    FUN_00036698(
        *(char **)((char *)g_41a4 + g_a50d * 4 + g_b072[param1 * 19] * 12),
        (unsigned short)(0xb8 + (0x118 - FUN_00036648(
            *(char **)((char *)g_41a4 + g_a50d * 4 + g_b072[param1 * 19] * 12),
            g_11be4, 0x54, -2, 8)) / 2),
        0x13a, 0xf, 0x54, g_11be4, -2, 8, 0, 0);

    /* section 2 */
    FUN_00036698(*(char **)((char *)g_469c + g_a50d * 4), 0xc2, 0x14e, 0xf, 0x166,
                 g_11be4, -2, 8, 0, 0);
    FUN_0003a4fa(buf, g_365c, *(unsigned int *)(g_539c + param1 * 10 + 6));
    FUN_00036698(buf, 0x10c, 0x14e, 0xf, 0x54, g_11be4, -2, 8, 0, 0);
    FUN_00036698(*(char **)((char *)g_46b4 + g_a50d * 4), 0xc2, 0x15c, 0xf, 0x166,
                 g_11be4, -2, 8, 0, 0);

    /* section 3: owner/claim gate */
    if ((unsigned short)g_539c[param1 * 10 + 2] == g_10b16
        || *(unsigned short *)(g_539c + param1 * 10) == 0xff) {
        FUN_000265d8(param1);
        FUN_00036698(*(char **)((char *)g_46a8 + g_a50d * 4), 0xc2, 0x16a, 0xf, 0x166,
                     g_11be4, -2, 8, 0, 0);
        if (*(unsigned short *)(g_539c + param1 * 10) == 0xff) {
            if (g_5392 > 0x15a && g_5392 < 0x174 && g_5390 > 0x10 && g_5390 < 0x92)
                colour = 0x166;
            else
                colour = 0x1ef;
            FUN_00036208(*(char **)((char *)g_45d0 + g_a50d * 4), 0x10, 0x15a, 0x82,
                         colour, -2, 0xc);
        }
    } else {
        FUN_00036698(*(char **)((char *)g_46cc + g_a50d * 4), 0x10c, 0x15c, 0xf, 0x54,
                     g_11be4, -2, 8, 0, 0);
        FUN_00036698(*(char **)((char *)g_46c0 + g_a50d * 4), 0xc2, 0x16a, 0xf, 0x166,
                     g_11be4, -2, 8, 0, 0);
        if (FUN_000264a8(param1) == 1) {
            if (g_5392 > 0x15a && g_5392 < 0x174 && g_5390 > 0x10 && g_5390 < 0x92)
                colour = 0x166;
            else
                colour = 0x1ef;
            FUN_00036208(*(char **)((char *)g_45d0 + g_a50d * 4), 0x10, 0x15a, 0x82,
                         colour, -2, 0xc);
        }
    }

    /* section 4: owner/claim gate -> status string */
    if ((unsigned short)g_539c[param1 * 10 + 2] == g_10b16
        || *(unsigned short *)(g_539c + param1 * 10) == 0xff) {
        claim = *(short *)(g_539c + param1 * 10);
        if (claim < 0x2f)
            FUN_0003a4fa(buf, *(char **)((char *)g_46d8 + g_a50d * 4));
        else if (claim < 0x5f)
            FUN_0003a4fa(buf, *(char **)((char *)g_46e4 + g_a50d * 4));
        else if (claim < 0xa0)
            FUN_0003a4fa(buf, *(char **)((char *)g_46f0 + g_a50d * 4));
        else if (claim < 0xd1)
            FUN_0003a4fa(buf, *(char **)((char *)g_46fc + g_a50d * 4));
        else if (claim < 0xfe)
            FUN_0003a4fa(buf, *(char **)((char *)g_4708 + g_a50d * 4));
        else
            FUN_0003a4fa(buf, *(char **)((char *)g_4714 + g_a50d * 4));
    } else {
        FUN_0003a4fa(buf, g_3660,
                     g_e49c + g_539c[param1 * 10 + 2] * 1047 + 0x23);
    }
    FUN_00036698(buf, 0x10c, 0x16a, 0xf, 0x54, g_11be4, -2, 8, 0, 0);
}
