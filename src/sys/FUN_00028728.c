/* PARKED near-miss (NOT matched, ~165/187) -- DPMI int 0x31 AX=0x100 DOS-memory alloc then
   far-pointer zero-fill. Instruction-exact incl. the inlined _fmemset (far rep stosw). Register-
   role wall: local_18 lands in ESI not EAX (target reuses the memset fill-byte reg + a final
   xor ah,ah as the &0xffff0000 mask, no spill); local_c in EDX not ECX. Not source-reachable. */
/* FUN_00028728 @ 0x28728 - DPMI (int 0x31, AX=0x100) allocate DOS memory block,
   then zero-fill the block through a far pointer. */

extern void FUN_0003aaf8(void *dst, int val, int len);   /* memset helper */
extern void int386(int a, void *msg, void *scratch);
extern void FUN_000289a8(int a, int b, int c);

extern void __far *_fmemset(void __far *dst, int c, unsigned n);
#pragma intrinsic(_fmemset)

unsigned int FUN_00028728(unsigned short *param_1, unsigned short param_2)
{
    int msg[7];
    int scratch[7];
    unsigned int local_18;
    int local_c;

    FUN_0003aaf8(msg, 0, 0x1c);
    FUN_0003aaf8(scratch, 0, 0x1c);
    msg[0] = 0x100;
    msg[1] = (param_2 + 0x10) / 16;
    int386(0x31, msg, scratch);
    local_c = scratch[6];
    if (local_c != 0) {
        FUN_000289a8(0x376c, 0x297, -2);
        return 0;
    }
    *param_1 = *(unsigned short *)scratch;
    local_18 = scratch[3];
    param_1[3] = (short)local_18;
    *(int *)(param_1 + 1) = local_c;
    _fmemset(*(void __far **)(param_1 + 1), 0, param_2);
    return local_18 & 0xffff0000;
}
