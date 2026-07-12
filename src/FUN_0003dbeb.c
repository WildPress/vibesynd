/* itoa-style signed formatter @ 0x0003dbeb (framed CLIB).
 * Body logic is byte-faithful: p=buf; if(radix==10 && val<0){val=-val;
 * p=buf+1; *buf='-';} call 0x3db9e(val,p,radix); return buf.
 *
 * WALL: prologue reg-save order. Target prologue is `push ebx; push ebp;
 * mov ebp,esp` (regs BEFORE frame, args at [EBP+0xc/0x10/0x14] -> EAX/EBX/ECX).
 * Watcom 9.5b optimized recipes emit frame-FIRST `push ebp; mov ebp,esp;
 * push ebx` (args at [EBP+8..], reg alloc into EDX/EBX with radix re-read from
 * memory), diverging at byte 0 and cascading through the whole body. Only -od
 * gives regs-first but de-optimizes. Same wall as unmatched sibling
 * FUN_0003d3e4. Not source-reachable. */
extern char *FUN_0003db9e(int val, char *dst, int radix);

char *FUN_0003dbeb(int val, char *buf, int radix)
{
    char *p = buf;
    if (radix == 10 && val < 0) {
        val = -val;
        p = buf + 1;
        *buf = '-';
    }
    FUN_0003db9e(val, p, radix);
    return buf;
}
