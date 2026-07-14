/* C runtime: close @ 0x3a89d (CLIB3S, Watcom 9.5). DOS close (AH=0x3E, BX=handle).
   int 21h; fold carry into the sign bit (rcl eax,1; ror eax,1); on success call the
   handle-release helper FUN_0003c57b(handle,0) and return 0; on error set *__errno=4
   (EBADF) via FUN_0003c46d and return -1.  Hand-asm: replicated as a full-body
   #pragma aux whose two calls are real masked relocs; -d2 supplies the frame and the
   handle sits at [ebp+0x10] (push ebx; push esi; push ebp; mov ebp,esp prologue). */
extern int  FUN_0003c57b(int handle, int flags);
extern int *FUN_0003c46d(void);
extern int  __close(void);
#pragma aux __close = "db 139" "db 117" "db 16" "db 102" "db 137" "db 243" "db 180" "db 62" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 133" "db 192" "jl short L2" "db 106" "db 0" "db 86" "call FUN_0003c57b" "db 131" "db 196" "db 8" "db 49" "db 192" "jmp short L3" "L2: call FUN_0003c46d" "db 199" "db 0" "db 4" "db 0" "db 0" "db 0" "db 184" "db 255" "db 255" "db 255" "db 255" "L3:" parm [] value [eax] modify exact [eax ebx ecx edx esi];

int FUN_0003a89d(int handle)
{
    return __close();
}
