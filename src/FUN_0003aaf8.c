/* C runtime: memset @ 0x3aaf8 (CLIB3S, Watcom 9.5). Hand-asm, FRAMELESS, stack-calling.
   Loads dst/c/len from [esp+4/8/0xc], splats byte c into all 4 bytes of EDX
   (mov dh,dl; shl edx,8; mov dl,dh; shl; mov dl,dh), then tails into the shared
   aligned fill core FUN_0003ca18 (eax=dst, edx=pattern, ecx=len) and returns dst.
   Emitted as a frameless wrapper whose whole body is the inlined #pragma aux __memset
   (db-transcribed, with a real masked `call FUN_0003ca18`). Compile WITHOUT -d2 so the
   wrapper stays frameless and the body reads [esp+N] exactly as the target does. */
extern void FUN_0003ca18(void);
extern void *__memset(void);
#pragma aux __memset = "db 139" "db 68" "db 36" "db 4" "db 139" "db 84" "db 36" "db 8" "db 139" "db 76" "db 36" "db 12" "db 138" "db 242" "db 193" "db 226" "db 8" "db 138" "db 214" "db 193" "db 226" "db 8" "db 138" "db 214" "call FUN_0003ca18" "db 139" "db 68" "db 36" "db 4" parm [] value [eax] modify exact [eax edx ecx];

void *FUN_0003aaf8(void *dst, int c, unsigned len)
{
    return __memset();
}
