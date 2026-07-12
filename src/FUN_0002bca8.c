/* WALL (inline jump-table dispatch) @ 0x2bca8  -- command-list interpreter.
 *
 * Ground truth: disassemble_function 0x2bca8 + disassemble_bytes 0x2bcd5..0x2becd.
 * Real signature is 4 stack params (Ghidra's 1-param __cdecl is phantom):
 *   FUN(short *p /[ESP+0x10]->EBX/, int sel /[ESP+0x14]->ESI/,
 *       int setX /[ESP+0x18]/, int setY /[ESP+0x1c]/)  -> returns 0.
 *
 * Walks a list of 0x12-byte command records; word[0] is the opcode, opcode
 * == -0x63 (0xff9d) terminates (return 0). Dispatch is a 5-way switch:
 *   MOV DX,[p]; CMP DX,4; JA default; JMP CS:[EAX*4 + 0x1e540]
 * Distinct case bodies (addresses are the block starts the far table selects):
 *   0x2bcdd  box hit-test of cursor point (g_10b22,g_10b24) against the record
 *            box [x=p[2]+p[8]..+p[6], y=p[4]+p[9]..+p[7]]; on hit (or when
 *            sel == (signed char)p[0xb]) sets selection cursor globals
 *            (g_e116/g_e114 = 0 or 1, g_10b3f/g_10b3e cleared per setX/setY),
 *            then opcode += 2 and p[0xa] = 2.
 *   0x2bdad  push p; if (FUN_00029988(p)) (*p)++;  -> next.
 *   0x2bdc7  if (p[0xa]) { di = (signed char)p[0xb]; } -> next.
 *   0x2bdd7  second box hit-test variant writing the same cursor globals; miss
 *            path 0x2be93 optionally push p; FUN_00029988(p); if hit *p = 1.
 *   0x2beb9  default: p += 0x12 (advance to next record), reload, loop.
 *
 * WHY NO MASKED 'YES' (playbook Section 0, cf. 0x23038): the 5-entry jump table
 * (+ entry-alignment pad) sits in a FAR segment in the shipped binary
 * (CS:[EAX*4 + 0x1e540]), so the on-disk function body is clean; but Watcom
 * co-locates that table inside our object .text ahead of the code. match_reloc
 * compares the whole object text against the target window, so len(ours) !=
 * len(target) can never resolve even were every code byte identical. No C
 * spelling moves the table out of the compiled object. Not source-reachable.
 *
 * MANIFEST SIZE UNDER-COUNTED: recorded size=74 truncates at the switch JMP;
 * true extent is 0x2bca8..0x2becd = 550 bytes (0x226).
 *
 * Parked as WALL. Reconstruction below is structural (opcode->block assignment
 * lives only in the masked far table and does not affect the function bytes).
 */
extern unsigned short g_10b22, g_10b24;   /* cursor point (x, y) */
extern unsigned short g_e114, g_e116;     /* selection cursor state words */
extern unsigned char  g_10b3e, g_10b3f;   /* selection cursor flags */
extern short FUN_00029988(short *p);

static int hit_test(short *p)
{
    int x = (unsigned short)p[1] + *(signed char *)((char *)p + 8);   /* p[2] + s8 p[8] */
    if ((short)g_10b22 <  x) return 0;
    x += *(unsigned char *)((char *)p + 6);
    if ((short)g_10b22 >= x) return 0;
    {
        int y = (unsigned short)p[2] + *(signed char *)((char *)p + 9); /* p[4] + s8 p[9] */
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
    *(short *)p += 2;
    *((char *)p + 0xa) = 2;
}

int FUN_0002bca8(short *p, int sel, int setX, int setY)
{
    for (; *p != -0x63; p = (short *)((char *)p + 0x12)) {
        switch ((unsigned short)*p) {          /* JMP CS:[..+0x1e540] (WALL) */
        case 0:                                 /* block 0x2bcdd */
            if ((setX | setY) &&
                (hit_test(p) || sel == *(signed char *)((char *)p + 0xb)))
                set_cursor(p, setX, setY);
            break;
        case 1:                                 /* block 0x2bdad */
            if (FUN_00029988(p))
                (*p)++;
            break;
        case 2:                                 /* block 0x2bdc7 */
            /* if (p[0xa]) sel = (signed char)p[0xb];  -- no state change */
            break;
        case 3:                                 /* block 0x2bdd7 */
            if ((setX | setY) && hit_test(p))
                set_cursor(p, setX, setY);
            else if (FUN_00029988(p))
                *p = 1;
            break;
        default:                                /* block 0x2beb9 */
            break;
        }
    }
    return 0;
}
