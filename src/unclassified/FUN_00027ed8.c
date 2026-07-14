/* frameless @ 0x27ed8: if(p1||p2) FUN_000287c8(p1,(unsigned short)p2). NEAR-MISS (parked).
   Logic exact but the target loads p2 (16-bit `mov ax,[esp+8]`) BEFORE p1 and no source form
   flips that scheduling (first diff at 0x0, 66 vs 8b). Instruction-scheduling wall. */
extern void FUN_000287c8(int a, int b);
void FUN_00027ed8(int p1, unsigned short p2)
{
    if (p1 != 0 || p2 != 0)
        FUN_000287c8(p1, p2);
}
