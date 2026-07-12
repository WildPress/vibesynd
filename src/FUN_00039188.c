/* frameless wrapper @ 0x39188 (25B): if the global flag byte g_10b49 is set,
   call FUN_000395b6(0x3d24) (cdecl, one stack arg). 0x3d24 is a literal (below
   image base 0x10000, not relocated). g_10b49 is the same flag byte tested by
   the matched sibling FUN_00038fe8.

   NEAR-MISS (23/25). The 23-byte body is byte-exact / reloc-aware:
     ours   :   803d490b010000 740d 68243d0000 e8<rel> 83c404 c3
     target : 53 803d490b010000 740d 68243d0000 e8<rel> 83c404 5b c3
   The ONLY divergence is the target's enclosing dead callee-save
   `push ebx` (53) ... `pop ebx` (5b): the target reserves EBX across the call
   although nothing is live in it. This whole cluster (0x395b6, 0x39722, 0x39495)
   pessimistically saves callee regs even when unused. Not source-reachable here:
   with -oneatx nothing is live across the call so Watcom saves no reg (frameless,
   23B); -od DOES save regs but then saves ebx+esi+edi AND adds a full ebp frame
   (push ebp;mov ebp,esp;sub esp;...;leave), i.e. way past the target's single
   frameless EBX save. No flag/source spelling yields exactly one frameless EBX
   save. §3 register-role/frame class. Winning body recipe: -4s -oneatx -zp8 -s -zq. */
extern char g_10b49;
extern void FUN_000395b6(int);

void FUN_00039188(void)
{
    if (g_10b49 != 0)
        FUN_000395b6(0x3d24);
}
