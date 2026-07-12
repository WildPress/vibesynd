/* NEAR-MISS 33/34 @ 0x27ed8 (recipe -4s -oneatx -zp8 -s -zq, the closest of all tried).
   Logic is byte-identical from offset 0x8 onward. Only the two leading param loads
   differ: target loads param_2 FIRST as a 16-bit MOV AX ([ESP+8]) then param_1 (MOV
   EDX,[ESP+4]); our compile loads param_1 first (MOV EDX) then param_2 as a 32-bit
   MOV EAX. Net: target 34B / ours 33B (missing 0x66 operand-size prefix) with the two
   loads swapped. Every guard spelling (||, De Morgan, temp-first) and every recipe
   (-or/-ot/-os/-ei) converge to this same order -> scheduling / param-load-order wall
   (§3, same class as 0x15e38). Thin wrapper: guard (param_1||param_2) then call 0x287e8,
   mirroring 0x287e8's own guard. */
extern void FUN_000287e8(int param_1, unsigned short param_2);

void FUN_00027ed8(int param_1, unsigned short param_2)
{
    if (param_1 != 0 || param_2 != 0)
        FUN_000287e8(param_1, param_2);
}
