/* FUN_00039280 (register/-4r). Table lookup: search key-record list.
 * params: a in EAX, index in EBX (3rd reg param => a,unused,idx).
 *
 * WALL: register-role tie-break (near-miss 42B ours vs 44B target, structure
 * correct). Best recipe: "-4r -oneatx -zp8 -s -zq" with the idx-reuse form
 * below -> EBX correctly reused as walker, control flow + all instructions
 * shaped right. Irreducible diffs (all allocator/peephole, not source-
 * reachable; identical across -3r / -4r / -or / -oneatx):
 *   - key `a` evicted EAX->ECX (push ecx;mov ecx,eax) while loaded v takes EAX;
 *     target keeps a in EAX, v in ECX (roles swapped).
 *   - our SIB `mov ebx,[ebx*4+disp]` vs target pre-scaled `shl,1;shl,1;
 *     mov ebx,[ebx+disp]`.
 *   - our `test ebx,ebx` / `xor eax,eax` vs target `cmp ebx,0` / `mov eax,0`.
 * target : 83fb10731dd1e3d1e38b9bfabc000083fb00740e8b0b3bc8740e83c30883f9ff75f2b800000000c38b4304c3
 * ours   : 5189c183fb10731e8b1c9d0000000085db74138b0339c875058b430459c383c30883f8ff75ed31c059c3
 *
 * disasm: if ((unsigned)idx >= 0x10) return 0;
 *          p = g_bcfa[idx]; if (!p) return 0;
 *         do { v = p[0]; if (v==a) return p[1]; p+=2; } while (v != -1);
 *         return 0;
 */
extern unsigned char g_bcfa[];

int FUN_00039280(int a, int b, int idx)
{
    int v;

    if ((unsigned)idx >= 0x10)
        return 0;
    idx = *(int *)(g_bcfa + idx * 4);
    if (idx == 0)
        return 0;
    do {
        v = *(int *)idx;
        if (v == a)
            return *(int *)(idx + 4);
        idx += 8;
    } while (v != -1);
    return 0;
}
