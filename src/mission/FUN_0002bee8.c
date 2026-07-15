/* inline jump-table switch dispatcher @ 0x2bee8 -- command-list interpreter,
 * THIRD sibling of FUN_0002bca8 / FUN_0002c218 (same 0x12-byte record walk,
 * word[0]==-0x63 terminator, same select-cursor blocks). 5-entry jump table at
 * obj1:+0x1e788 (manifest 0x2bed0, verified via lefix.py):
 *   case 0 -> 0x2c153 (= default, empty)     case 3 -> 0x2c04a (active-check/box)
 *   case 1 -> 0x2bf1d (box-test + activate)  case 4 -> 0x2c13a (helper, set 1)
 *   case 2 -> 0x2c030 (helper, increment)
 * Case map is the 0x2bca8 map shifted +1 (0=empty, 1=box, 2=inc, 3=check, 4=set1).
 *
 * Case 1 has an EXTRA gate the siblings lack: after box/sel selection it checks
 * the commanded agent's flag byte -- agent rec = g_pool_a + 0x5c*(p[0xb]-1 +
 * g_e551[g_cur_player*0x417]) must have bit 4 set at +0x1d, else skip. On activation
 * it sets the cursor state words (g_sel_cursor/g_e114 + g_10b3f/g_10b3e) and advances
 * the command word by 2. Case 3 adds a g_e120==2 lockout before writing cmd=1.
 * Post-switch tail (when p[0xa]!=0, or unconditionally from active cases):
 * clears TEN per-record flag bytes g_df48..g_df51 (each an 11-stride column,
 * index (p[0xb]-1)*0xb, written in target order 9,0,2,1,3,5,4,6,8,7).
 *
 * TRUE SIZE: 792 bytes (0x2bee8..0x2c1ff inclusive; manifest's 226 is a big
 * undercount -- it stops mid-case-1). Jump table = 20 bytes at 0x2bed0.
 * Recipe: -4s -oneatx -zp8 -s -zq (sibling recipe; -ox required for box tests).
 *
 * NEAR-MISS 800/792 code bytes -- the SAME three -ox walls the siblings parked
 * on, nothing else (case 3, the tail clears, cases 0/2/4, the box tests and
 * the A_58 agent-flag gate are all byte-exact):
 *   1. case-1's two `=1` cursor-word stores emit `mov reg,1; mov [g],reg`
 *      where the target has immediate `mov word [g],1` (+2 each => +8 total
 *      with fallout). The NEW VOLATILE-ALIAS lever (g_e116v/g_e114v below)
 *      fixed the IDENTICAL shape in case 3 completely (store forms, the
 *      setY memory-cmp, both immediates) but does nothing in case 1; decl-
 *      volatile on the real names regresses case 1 further (kills the DH/CH
 *      param loads). Volatile through a CAST is ignored by wcc386 9.5 --
 *      only the declaration's qualifier counts.
 *   2. entry: `xor edi,edi` schedules after the guard movsx and the early
 *      return folds `mov eax,edi` -> `xor eax,eax` (net 0).
 *   3. loop tail: `movsx eax,[ebx+0x12]; add ebx,0x12` vs target
 *      `add ebx,0x12; movsx eax,[ebx]` (net 0; volatile-deref cast and
 *      `*(short *)(p += 0x12)` both fail to flip it).
 * Case-1 x-block needs ushort+schar operand order (schar->EDX); the y-block
 * and both case-3 blocks keep the sibling spellings. g_e116v/g_e114v are
 * link-time ALIASES of g_e114/g_sel_cursor (fixup masking ignores names): declare
 * the same word volatile ONLY where the volatile codegen is wanted.
 */
extern volatile unsigned short g_cursor_x, g_cursor_y;   /* cursor point (x, y) */
extern unsigned short g_e114, g_sel_cursor;     /* selection cursor state words */
extern volatile unsigned short g_e114v, g_e116v;  /* same words, volatile view (case 3) */
extern unsigned short g_e120;
extern unsigned char  g_10b3e, g_10b3f;   /* selection cursor flags */
extern short g_cur_player;                     /* current player index */
extern unsigned char g_e551[];            /* per-player block (0x417 stride) */
extern unsigned char g_pool_a[];            /* agent records (0x5c stride) */
extern unsigned char g_df48[], g_df49[], g_df4a[], g_df4b[], g_df4c[];
extern unsigned char g_df4d[], g_df4e[], g_df4f[], g_df50[], g_df51[];
extern short FUN_00029988(unsigned char *p);

unsigned short FUN_0002bee8(unsigned char *p, int sel, unsigned char setX, unsigned char setY)
{
    unsigned short result = 0;

    if (*(short *)p == -0x63)
        return result;
    do {
        switch (*(unsigned short *)p) {
        default:
        case 0:
            goto Lcheck;

        case 1:                                 /* block 0x2bf1d */
            if ((setX | setY) == 0) goto A_47;
            {
                int x = (unsigned short)*(unsigned short *)(p + 2) + *(signed char *)(p + 8);
                if (g_cursor_x < x) goto A_47;
                x += p[6];
                if (g_cursor_x >= x) goto A_47;
            }
            {
                int y = (unsigned short)*(unsigned short *)(p + 4) + *(signed char *)(p + 9);
                if (g_cursor_y < y) goto A_47;
                y += p[7];
                if (g_cursor_y < y) goto A_58;
            }
        A_47:
            if ((unsigned short)sel != *(signed char *)(p + 0xb)) goto Lcheck;
        A_58:
            {
                int c = g_e551[(int)g_cur_player * 0x417];
                unsigned char *a = g_pool_a + (*(signed char *)(p + 0xb) - 1 + c) * 0x5c;
                if ((a[0x1d] & 4) == 0) goto Lcheck;
            }
            g_sel_cursor = 0;
            if (setX) { g_10b3f = 0; g_e116v = 1; }
            g_e114 = 0;
            if (setY) { g_10b3e = 0; g_e114v = 1; }
            {
                short t = *(short *)p;
                p[0xa] = 2;
                *(short *)p = t + 2;
            }
            goto Lclear;

        case 2:                                 /* block 0x2c030 */
            if (FUN_00029988(p))
                (*(short *)p)++;
            p[0xa] = 2;
            goto Lclear;

        case 3:                                 /* block 0x2c04a */
            if (p[0xa] != 0) {
                result = *(signed char *)(p + 0xb);
                goto Lcheck;
            }
            if ((setX | setY) == 0) goto C_93;
            {
                int x = (unsigned short)*(unsigned short *)(p + 2) + *(signed char *)(p + 8);
                if (g_cursor_x < x) goto C_93;
                x += p[6];
                if (g_cursor_x >= x) goto C_93;
            }
            {
                int y = (unsigned short)*(unsigned short *)(p + 4) + *(signed char *)(p + 9);
                if (g_cursor_y < y) goto C_93;
                y += p[7];
                if (g_cursor_y >= y) goto C_93;
            }
            g_e116v = 0;
            if (setX) { g_10b3f = 0; g_e116v = 1; }
            g_e114v = 0;
            if (setY) { g_10b3e = 0; g_e114v = 1; }
            p[0xa] = 2;
            goto Lclear;
        C_93:
            if ((unsigned short)sel == *(signed char *)(p + 0xb)) goto Lcheck;
            if (g_e120 == 2) goto Lcheck;
            *(short *)p = 1;
            p[0xa] = 2;
            goto Lclear;

        case 4:                                 /* block 0x2c13a */
            if (FUN_00029988(p))
                *(short *)p = 1;
            p[0xa] = 2;
            goto Lclear;
        }
    Lcheck:                                     /* 0x2c153 */
        if (p[0xa] == 0) goto Lnext;
    Lclear:                                     /* 0x2c15d */
        g_df51[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
        g_df48[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
        g_df4a[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
        g_df49[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
        g_df4b[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
        g_df4d[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
        g_df4c[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
        g_df4e[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
        g_df50[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
        g_df4f[(*(signed char *)(p + 0xb) - 1) * 0xb] = 0;
    Lnext:                                      /* 0x2c1eb */
        p += 0x12;
    } while (*(short *)p != -0x63);

    return result;
}
