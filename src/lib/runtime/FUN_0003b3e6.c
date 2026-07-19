/* C runtime: int386x @ 0x3b3e6 (CLIB3S, Watcom 9.5). Register-setup wrapper for a
   software interrupt: saves ebx/esi/edi/ebp, loads the four args (inter_no, in,
   out, sregs) into ESI/EDI/EDX/EBX, calls the core int dispatcher int386x,
   then returns *out (the resulting AX/EAX packed into the out-regs). This is the
   int386x that int386 (0x3adb2) tail-calls. Regs-first prologue (push reg before
   the ebp frame) -> the reg-save-order wall for plain C, so it is whole-function
   db-transcribed into a frameless wrapper; the trailing RET comes from the wrapper. */
extern void int386x(void);
extern int __int386x(void);
#pragma aux __int386x = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 117" "db 20" "db 139" "db 125" "db 24" "db 139" "db 85" "db 28" "db 139" "db 93" "db 32" "call int386x" "db 139" "db 69" "db 28" "db 139" "db 0" "db 93" "db 95" "db 94" "db 91" parm [] value [eax] modify exact [eax ecx edx];
int FUN_0003b3e6(int inter_no, const void *in, void *out, void *sregs)
{
    return __int386x();
}
