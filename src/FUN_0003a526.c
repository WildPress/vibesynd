/* C runtime: atol @ 0x3a526 (CLIB3S). Size 83B. NEAR-MISS (parked).
   Logically exact and byte-length-correct; the ONLY divergence is a whole-body EBX<->EDX
   register swap. The target keeps the accumulator r in EBX (31 db / 6b db / f7 db / 89 d8);
   Watcom global-CSEs the repeated inline ctype index `__ctype[(uchar)(*s+1)]` into the callee-
   saved EBX, stealing it from r, which lands in EDX. First diff at 0x28 (target 31 db, ours 31 d2).
   Confirmed unbreakable: ~35 hand variants + a 33,000-variant cpermute search, no -o flag flips it.
   Register tie-break wall (playbook S3; cf. docs/register-allocation.md). ctype table = extern reloc. */
extern unsigned char __ctype[];
long FUN_0003a526(char *s)
{
    char sign;
    long r;
    while (__ctype[(unsigned char)(*s + 1)] & 2) s++;
    sign = *s;
    if (sign == '+' || sign == '-') s++;
    r = 0;
    while (__ctype[(unsigned char)(*s + 1)] & 0x20) {
        r = r * 10 + (unsigned char)*s;
        s++;
        r -= '0';
    }
    if (sign == '-') r = -r;
    return r;
}
