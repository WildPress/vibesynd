/* MATCHED @ 0x2d9e8 (853/853, reloc-aware) -- recipe -4s -oneatx -zp8 -s -zq.
 * Levers that closed it: (1) decl order nb-BEFORE-ca steers ca into ECX /
 * nb into EBX (ca-first gave ca=EBX and rotated the divisor regs); (2) the
 * +0x1c flag byte must be read INLINE in every test (a named uchar local
 * homes AL everywhere; inline CSEs into DH / DL / AL per chain and emits the
 * 3-byte `test dh,imm` forms); (3) counter scaling written as COMPOUND
 * `ca *= w+1` gives the in-place `and ecx,0xffff; imul ecx,eax` (plain
 * `ca = ca * ...` splits into an xor-form EDX temp); (4) `end` read through
 * a VOLATILE alias extern of g_10ae0 pins the `mov ebp,[g]` load FIRST at
 * entry (non-volatile, the scheduler sinks it below the id_a chain); the
 * done-store uses the normal name. A named `w` for the +0x20 re-read
 * REGRESSED block 1 (reuse-copy instead of the fresh re-load) -- the plain
 * inline `&&` guard reproduces the target's double read exactly.
 *
 * squad_threat_test @ 0x2d9e8 (853B, leaf). Pool-A squad interference / threat
 * test for record pair (a, b): pool A records are 0x5c bytes at g_pool_a
 * (id = ptr - g_entity_pool), grouped 8-per-squad (0x2e0 bytes). Returns 0 at once
 * if b's link target (+0x20 id) or b itself lies in a's 8-record squad
 * (signed (ptr - g_pool_a)/0x5c/8 group compare). Otherwise scans the whole
 * pool scoring members whose +0x20 id maps into a squad of interest
 * (unsigned (id-2)/0x2e0 group), skipping records with +0xb bit0 set, by
 * their +0x1c flag byte: bit1->5, bit2->4, bit3->3, bit4->2, bit0->1.
 * If b has +0x1c bit1 (branch A) both a's and b's squads are scored (ca/cb),
 * each scaled by ((+0x3c & 0x600) >> 9) + 1 and incremented; returns 1 iff
 * ca >= 0x20 and ca > cb. Otherwise (branch B, weights 5/2/2/1/1) only a's
 * squad is scored and the result is a ladder on b's +0x1c flags:
 * bit0/ca>=1, bit4/ca>=2, bit3/ca>=4, bit2/ca>=8 -> 1, else 0. The pool-end
 * pointer g_10ae0 is loaded at entry and re-stored (unchanged) at every
 * exit -- single-exit source, Watcom tail-duplicates the return-1 blocks. */
extern unsigned char g_entity_pool[];
extern unsigned char g_pool_a[];
extern unsigned char *g_10ae0;
extern unsigned char * volatile g_10ae0v;   /* volatile alias of g_10ae0: pins the
                                               entry load first (fixups are masked,
                                               so the alias symbol is free) */

int squad_threat_test(unsigned char *a, unsigned char *b)
{
    unsigned short id_a;
    unsigned short id_b;
    unsigned char *end = g_10ae0v;
    unsigned char *nb;
    unsigned short ca;
    unsigned short cb;
    unsigned int g;
    int r;

    id_a = a - g_entity_pool;
    ca = 0;
    cb = 0;
    if (*(unsigned short *)(b + 0x20) != 0 &&
        (g_entity_pool + *(unsigned short *)(b + 0x20) - g_pool_a) / 0x5c / 8 ==
        (a - g_pool_a) / 0x5c / 8)
        goto fail;
    if ((b - g_pool_a) / 0x5c / 8 == (a - g_pool_a) / 0x5c / 8)
        goto fail;
    if (*(b + 0x1c) & 2) {
        id_b = b - g_entity_pool;
        for (nb = g_pool_a; nb < end; nb += 0x5c) {
            if ((*(nb + 0xb) & 1) == 0) {
                g = (*(unsigned short *)(nb + 0x20) - 2) / 0x2e0u;
                if (g == (id_b - 2) / 0x2e0u) {
                    if (*(nb + 0x1c) & 2)
                        cb += 5;
                    else if (*(nb + 0x1c) & 4)
                        cb += 4;
                    else if (*(nb + 0x1c) & 8)
                        cb += 3;
                    else if (*(nb + 0x1c) & 0x10)
                        cb += 2;
                    else if (*(nb + 0x1c) & 1)
                        cb += 1;
                } else if (g == (id_a - 2) / 0x2e0u) {
                    if (*(nb + 0x1c) & 2)
                        ca += 5;
                    else if (*(nb + 0x1c) & 4)
                        ca += 4;
                    else if (*(nb + 0x1c) & 8)
                        ca += 3;
                    else if (*(nb + 0x1c) & 0x10)
                        ca += 2;
                    else if (*(nb + 0x1c) & 1)
                        ca += 1;
                }
            }
        }
        ca *= ((unsigned short)(*(unsigned short *)(a + 0x3c) & 0x600) >> 9) + 1;
        cb *= ((unsigned short)(*(unsigned short *)(b + 0x3c) & 0x600) >> 9) + 1;
        ca++;
        cb++;
        if (ca < 0x20)
            goto fail;
        if (ca <= cb)
            goto fail;
        r = 1;
        goto done;
    } else {
        for (nb = g_pool_a; nb < end; nb += 0x5c) {
            g = (*(unsigned short *)(nb + 0x20) - 2) / 0x2e0u;
            if (g == (id_a - 2) / 0x2e0u && (*(nb + 0xb) & 1) == 0) {
                if (*(nb + 0x1c) & 2)
                    ca += 5;
                else if (*(nb + 0x1c) & 4)
                    ca += 2;
                else if (*(nb + 0x1c) & 8)
                    ca += 2;
                else if ((*(nb + 0x1c) & 0x10) || (*(nb + 0x1c) & 1))
                    ca += 1;
            }
        }
        ca *= ((unsigned short)(*(unsigned short *)(a + 0x3c) & 0x600) >> 9) + 1;
        ca++;
        if ((*(b + 0x1c) & 1) && ca >= 1) {
            r = 1;
            goto done;
        }
        if ((*(b + 0x1c) & 0x10) && ca >= 2) {
            r = 1;
            goto done;
        }
        if ((*(b + 0x1c) & 8) && ca >= 4) {
            r = 1;
            goto done;
        }
        if ((*(b + 0x1c) & 4) && ca >= 8) {
            r = 1;
            goto done;
        }
    }
fail:
    r = 0;
done:
    g_10ae0 = end;
    return r;
}
