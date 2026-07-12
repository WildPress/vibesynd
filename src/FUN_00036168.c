/* frameless @ 0x36168: if(g_10b4a) FUN_0000344e(0);  NEAR-MISS (parked).
   Body matches (masked), but the target has a DEAD push ebx/pop ebx around the call
   (ebx never used) that no recipe reproduces -- the cluster dead-callee-save wall,
   sibling of the parked 0x39188 (same guard-then-call shape). */
extern unsigned char g_10b4a;
extern void FUN_0000344e(int);
void FUN_00036168(void)
{
    if (g_10b4a != 0)
        FUN_0000344e(0);
}
