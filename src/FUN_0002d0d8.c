/* jump-table dispatcher @ 0x2d0d8 -- rate-driven byte drift (3-channel value
 * fade). 4-entry jump table at obj1:+0x1f980 (manifest 0x2d0c8, via lefix.py):
 *   case 0 -> 0x2d118 (b=4,    a=0x14)   case 2 -> 0x2d131 (b=0x10, a=0x28)
 *   case 1 -> 0x2d124 (b=8,    a=0x1c)   case 3 -> 0x2d141 (b=0x28, a=0x80)
 * mode picks a slow rate a and fast rate b (b spills to the stack word local).
 *
 * If force != 0: *p1 = force+0x80, *p3 = force, done. Otherwise every tick
 * where counter % a == 0, step *p3 one unit toward *p1 (or toward 0x80 when
 * p1==p2==p3 already agree); every tick where counter % b == 0, step *p2 one
 * unit toward *p1, and when they are already equal step BOTH *p1 and *p2 one
 * unit toward *p3. Steps are three-way signs (-1/0/+1).
 *
 * TRUE SIZE: 333 bytes (0x2d0d8..0x2d224 inclusive; manifest's 278 stops in
 * the second sign block). Jump table = 16 bytes at 0x2d0c8.
 * Recipe: -4s -oneatx -zp8 -s -zq
 *
 * NEAR-MISS 333/333 length-exact, 331/333 bytes correct (masked): everything
 * matches except the block-1 merge add, ONE commutative-add destination tie:
 *   target: add edx,eax; mov [ebx],dl   ours: add eax,edx; mov [ebx],al
 * (both operands dying; ours folds the sum into s's EAX, target into the
 * *p3-load EDX). Tried: v-temp, d-reuse (collapses the else branch), staged
 * `v=*p3; v+=s` (mirrors: copies s to EDX first), uchar temp (drops to byte
 * adds). Same class as the 0x272b8 register-tie the permuter cracked.
 * LEVERS THAT GOT HERE (playbook-worthy):
 *   - split-statement subtraction `int t = *a; t -= *b;` pins the difference
 *     to t's own home and kills the mirrored `sub edx,eax; mov eax,edx` copy
 *     (closed blocks 2 and 3);
 *   - `if (b != 0) goto Lb; ... return; Lb:` reproduces the target's
 *     else-first layout (identical output to swapped if-arms);
 *   - FRESH sign temp per block (t2, t3) -- one shared `s` across blocks webs
 *     everything into EDX-home with `setg al; xor edx,..; mov dl,al` hacks;
 *   - two-var `d`/`s` in block 1 keeps the target's `mov eax,edx` copy.
 */
void FUN_0002d0d8(unsigned char *p1, unsigned char *p2, unsigned char *p3,
                  unsigned char force, unsigned char counter, unsigned char mode)
{
    short a;
    short b;
    int s;
    int d;

    if (force != 0) {
        *p1 = (unsigned char)(force + 0x80);
        *p3 = force;
        return;
    }
    switch (mode) {
    case 0: b = 4;    a = 0x14; break;
    case 1: b = 8;    a = 0x1c; break;
    case 2: b = 0x10; a = 0x28; break;
    case 3: b = 0x28; a = 0x80; break;
    }
    if (counter % a == 0) {
        if (*p3 == *p1 && *p3 == *p2) {
            d = 0x80 - *p3;
            s = d < 0 ? -1 : d > 0;
        } else {
            d = *p1 - *p3;
            s = d < 0 ? -1 : d > 0;
        }
        {
            int v = *p3 + s;
            *p3 = (unsigned char)v;
        }
    }
    if (counter % b == 0) {
        int t2 = *p1;
        t2 -= *p2;
        t2 = t2 < 0 ? -1 : t2 > 0;
        b = (short)t2;
        if (b != 0) goto Lb;
        {
            int t3 = *p3;
            t3 -= *p1;
            t3 = t3 < 0 ? -1 : t3 > 0;
            *p2 += t3;
            *p1 += t3;
        }
        return;
    Lb:
        *p2 += b;
    }
}
