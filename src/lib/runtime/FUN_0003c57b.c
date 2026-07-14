/* C runtime: handle-release helper @ 0x3c57b (CLIB3S, Watcom 9.5). Stores the handle
   (arg1) OR'd with 0x40 in its high byte into the fd/handle table at ds:0xc07c indexed
   by arg2. Hand-asm: full-body #pragma aux; -d2 supplies the frame (push ebx; push ebp;
   mov ebp,esp), args at [ebp+0xc]/[ebp+0x10]. The 0xc07c ref is a literal absolute addr. */
extern void __hrel(void);
#pragma aux __hrel = "db 139" "db 93" "db 12" "db 139" "db 85" "db 16" "db 161" "db 124" "db 192" "db 0" "db 0" "db 128" "db 206" "db 64" "db 137" "db 20" "db 152" parm [] modify exact [eax ebx edx];

void FUN_0003c57b(int handle, int flags)
{
    __hrel();
}
