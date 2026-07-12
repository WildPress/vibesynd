/* WALL (inline jump-table dispatch) @ 0x2c218  -- command-list interpreter.
 *   Sibling of FUN_0002bca8 (same shape; distinct case-2 clause + default tail).
 *
 * Ground truth: disassemble_function 0x2c218 + disassemble_bytes 0x2c245..0x2c467.
 * Real signature is 4 stack params (Ghidra's 1-param __cdecl is phantom):
 *   FUN(short *p /[ESP+0x10]->EBX/, int sel /[ESP+0x14]->EDI/,
 *       int setX /[ESP+0x18]/, int setY /[ESP+0x1c]/)  -> returns 0.
 *   (Register roles vs 0x2bca8 are swapped: here EDI=sel, ESI=return-0.)
 *
 * Walks 0x12-byte command records; word[0]==-0x63 terminates (return 0).
 * Dispatch: MOV DX,[p]; CMP DX,4; JA default; JMP CS:[EAX*4 + 0x1eab8].
 * Case bodies (block starts the far table selects):
 *   0x2c24d  box hit-test of cursor (g_10b22,g_10b24) against record box, sets
 *            selection cursor globals (g_e116/g_e114, g_10b3f/g_10b3e), then
 *            opcode += 2, p[0xa] = 2  (same as 0x2bca8 case 0).
 *   0x2c31d  push p; if (FUN_00029988(p)) (*p)++; p[0xa]=2  -> shared tail.
 *   0x2c337  if (p[0xa]) { if (sel) -> next; else sel = (signed char)p[0xb]; }
 *            (extra `TEST SI,SI` clause absent in 0x2bca8).
 *   0x2c350  second box hit-test variant; miss path 0x2c410 optionally
 *            FUN_00029988(p) then *p = 1; sets same cursor globals; p[0xa]=2.
 *   0x2c441  DEFAULT / shared tail: if (p[0xa]) g_df75[(signed char)p[0xb]] = 0;
 *            then p += 0x12 and loop.  <-- the extra write vs 0x2bca8.
 *
 * WHY NO MASKED 'YES' (playbook Section 0, cf. 0x23038): the 5-entry jump table
 * (+ pad) lives in a FAR segment in the shipped binary (CS:[EAX*4 + 0x1eab8]);
 * Watcom instead co-locates it inside our object .text before the code, so
 * match_reloc's len(ours) != len(target) can never resolve even with byte-exact
 * code. Not source-reachable.
 *
 * MANIFEST SIZE UNDER-COUNTED: recorded size=92 truncates at the switch JMP;
 * true extent is 0x2c218..0x2c467 = 592 bytes (0x250).
 *
 * Parked as WALL. Reconstruction below is structural (opcode->block map lives in
 * the masked far table and does not affect the function bytes).
 */
extern unsigned short g_10b22, g_10b24;   /* cursor point (x, y) */
extern unsigned short g_e114, g_e116;     /* selection cursor state words */
extern unsigned char  g_10b3e, g_10b3f;   /* selection cursor flags */
extern unsigned char  g_df75[];           /* record-indexed flag table (default tail clears) */
extern short FUN_00029988(short *p);

static int hit_test(short *p)
{
    int x = (unsigned short)p[1] + *(signed char *)((char *)p + 8);
    if ((short)g_10b22 <  x) return 0;
    x += *(unsigned char *)((char *)p + 6);
    if ((short)g_10b22 >= x) return 0;
    {
        int y = (unsigned short)p[2] + *(signed char *)((char *)p + 9);
        if ((short)g_10b24 <  y) return 0;
        y += *(unsigned char *)((char *)p + 7);
        if ((short)g_10b24 >= y) return 0;
    }
    return 1;
}

static void set_cursor(short *p, int setX, int setY)
{
    g_e116 = 0;
    if (setX) { g_10b3f = 0; g_e116 = 1; }
    g_e114 = 0;
    if (setY) { g_10b3e = 0; g_e114 = 1; }
}

int FUN_0002c218(short *p, int sel, int setX, int setY)
{
    for (; *p != -0x63; p = (short *)((char *)p + 0x12)) {
        switch ((unsigned short)*p) {          /* JMP CS:[..+0x1eab8] (WALL) */
        case 0:                                 /* block 0x2c24d */
            if ((setX | setY) &&
                (hit_test(p) || sel == *(signed char *)((char *)p + 0xb))) {
                set_cursor(p, setX, setY);
                *(short *)p += 2;
            }
            *((char *)p + 0xa) = 2;
            break;
        case 1:                                 /* block 0x2c31d */
            if (FUN_00029988(p))
                (*p)++;
            *((char *)p + 0xa) = 2;
            break;
        case 2:                                 /* block 0x2c337 */
            /* if (p[0xa]) { if (!sel) sel = (signed char)p[0xb]; }  -- no state change */
            break;
        case 3:                                 /* block 0x2c350 */
            if ((setX | setY) && hit_test(p))
                set_cursor(p, setX, setY);
            else if (FUN_00029988(p))
                *p = 1;
            *((char *)p + 0xa) = 2;
            break;
        default:                                /* block 0x2c441 (shared tail) */
            break;
        }
        if (*((char *)p + 0xa))                 /* shared tail 0x2c441/0x2c447 */
            g_df75[*(signed char *)((char *)p + 0xb)] = 0;
    }
    return 0;
}
