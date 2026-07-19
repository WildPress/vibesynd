/* NEAR-MISS (NOT matched). EDIT-DIST=44 (was 52). Prologue now EXACT: returning
   `scratch[3] & 0xffff0000` directly (no held local_18) drops the ESI spill, so we
   push only EBX+EDI like the target -- 2 callee-saved, not 3.
   dos_alloc_and_zero: DPMI int 0x31 AX=0x100 DOS-memory alloc, then far-pointer
   zero-fill of the block via inlined _fmemset (far rep stosw/stosb).
   Two residual codegen-ties remain: (1) target keeps local_c in ECX (seeding the
   rep-count reg with the known-0 value); our base 9.5 puts it in EDX, needing an
   extra `xor ecx,ecx`. (2) After the fill, EAX already holds scratch[3]&0xffff0000
   (the fill zeroed AX in place), so the target just re-masks with `xor ah,ah`; our
   compiler doesn't see the value survives and reloads scratch[3]. Both are the
   smarter-allocator floor (9.5b-class), not source-reachable. */
/* dos_alloc_and_zero @ 0x28728 - DPMI (int 0x31, AX=0x100) allocate DOS memory block,
   then zero-fill the block through a far pointer. Proposed name: dos_alloc_and_zero. */

extern void memset(void *dst, int val, int len);   /* memset helper */
extern void int386(int a, void *msg, void *scratch);
extern void report_net_status(int a, int b, int c);

extern void __far *_fmemset(void __far *dst, int c, unsigned n);
#pragma intrinsic(_fmemset)

unsigned int dos_alloc_and_zero(unsigned short *param_1, unsigned short param_2)
{
    int msg[7];
    int scratch[7];
    int local_c;

    memset(msg, 0, 0x1c);
    memset(scratch, 0, 0x1c);
    msg[0] = 0x100;
    msg[1] = (param_2 + 0x10) / 16;
    int386(0x31, msg, scratch);
    local_c = scratch[6];
    if (local_c != 0) {
        report_net_status(0x376c, 0x297, -2);
        return 0;
    }
    *param_1 = *(unsigned short *)scratch;
    param_1[3] = (short)scratch[3];
    *(int *)(param_1 + 1) = local_c;
    _fmemset(*(void __far **)(param_1 + 1), 0, param_2);
    return scratch[3] & 0xffff0000;
}
