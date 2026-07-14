/* frameless @ 0x34048: snap a direction value `cur` toward target `tgt`.
   m = cur with byte-1 cleared (cur & 0xffff00ff); d = (short)(m - tgt). Clamp:
   d < -0x20 -> cur+0x20; d > 0x20 -> cur-0x20; else cur-d (r, computed eagerly
   into ebx); d==0 -> cur. Watcom keeps cur live in eax, forms m/d in edx, and
   precomputes ebx = cur - d.

   NEAR-MISS (~52/56, NOT matched -> not in recipes/manifest). The register-ROLE
   issue the earlier note called the wall IS SOLVED here: the accumulator form
   (reassign `cur`, single `return cur`) keeps cur in EAX and d in EDX exactly like
   the target, and the entire tail (0x27..0x37) is now BYTE-IDENTICAL. Two pure
   encoding tie-breaks remain, both unreachable from source with Watcom 9.5b:

   1. byte 0xc: target `30 e6` (xor dh,ah, cross-byte) vs ours `30 f6` (xor dh,dh,
      self-zero). Both clear byte-1 of the cur copy; the target references the
      source reg's ah, ours self-zeroes. Every role-correct spelling self-zeroes;
      the cross form only appears bundled with the swapped register role.
   2. the add: target encodes cur+0x20 as the accumulator imm32 form `05 20000000`
      (5 bytes) while ours always emits the imm8 form `83 c0 20` (3 bytes) -- same
      class as the 0x16678 push-imm8-vs-imm32 peephole wall. This 2-byte shortfall
      also shifts the two preceding JGE displacements (7d0f->7d0d, 7d14->7d12).
      Note the target's sub path uses the imm8 form `83 e8 20`, so add/sub are
      asymmetric in the target -- a peephole state 9.5b won't reproduce.

   In my compiler the correct-role layout is CORRELATED with (self-xor + imm8-add)
   and the cross-xor/imm32 forms only occur with the swapped role; the target
   breaks that correlation. No source form or recipe (-oneatx/-ot/-os/-or/-oi/none)
   crosses it. Genuine encoding-tie-break wall, same family as 0x34088 / 0x26e18. */
int FUN_00034048(int cur, int tgt)
{
    int m = cur & 0xffff00ff;
    int d = m - tgt;
    if ((short)d < 0) {
        if ((short)d < -0x20)
            cur += 0x20;
        else
            cur -= d;
    } else if ((short)d > 0) {
        if ((short)d > 0x20)
            cur -= 0x20;
        else
            cur -= d;
    }
    return cur;
}
