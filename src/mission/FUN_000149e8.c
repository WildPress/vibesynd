/* MATCHED (pending manifest size fix) @ 0x149e8 -- jump-table dispatcher,
 * 10-entry table at 0x149c0 (jmp CS:[ebx*4+0x7278] -> manifest 0x7278+0xd748;
 * table sits directly BEFORE the function, NOT at 0x15920 as old notes said).
 * TRUE SIZE 91 (0x5b): 0x149e8..0x14a42 incl. Manifest size 35 truncates the
 * compare window; ours' 91-byte code tail is byte-identical to the target
 * modulo the 3 masked fixups (table literal, call rel32, g_entity_pool imm).
 *
 * RECIPE: -4s -or -zp8 -s -zq  (NOT -oneatx: -oneatx homes b in EBX with the
 * switch selector in DL; -or keeps b in EDX / selector in BL like the target.
 * Named ushort id local is load-bearing for the and-form widen in case 8/9/10.)
 *
 * Command/effect applier: switch on the command type byte b[7] (cases 1..10):
 *   3      -> clear target ptr a+0x10, a[6]=0xff, a[5]=0
 *   4      -> FUN_00014828(a)
 *   7      -> a[6] = b[9]
 *   8,9,10 -> a+0x10 = pool-A node pointer g_entity_pool + word b[9]
 *   1,2,5,6 and out-of-range -> nothing.
 */
extern unsigned char g_entity_pool[];
extern void FUN_00014828(unsigned char *a);

void FUN_000149e8(unsigned char *a, unsigned char *b)
{
    switch (b[7]) {
    case 3:
        *(int *)(a + 0x10) = 0;
        a[6] = 0xff;
        a[5] = 0;
        return;
    case 7:
        a[6] = b[9];
        return;
    case 4:
        FUN_00014828(a);
        return;
    case 8:
    case 9:
    case 10:
    {
        unsigned short id = *(unsigned short *)(b + 9);
        *(unsigned char **)(a + 0x10) = g_entity_pool + id;
        break;
    }
    case 1:
    case 2:
    case 5:
    case 6:
        break;
    }
}
