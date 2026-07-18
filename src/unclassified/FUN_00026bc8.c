/* FUN_00026bc8 @ 0x26bc8 -- aim-step clamp. If the requested step length param_1
   is within the (s2,s3) vector magnitude, resolve a direction index via 0x4d221 and
   advance g_aim_x/g_aim_y along g_dir_d{x,y}[idx]*param_1>>8; otherwise step by the
   raw (param_2,param_3) and return -1.

   NEAR-MISS (dist=1, NOT matched). Register-role + zero-extend-idiom tie. Baseline
   (`unsigned idx`): 171B vs 170B, first diff at 0x27 -- registers match through the
   magnitude test, but `idx=(unsigned short)r` emits `mov ecx,eax; and ecx,0xffff`
   (idx->ECX) where the target uses the compact `xor edx,edx; mov dx,ax` (idx->EDX).
   Typing idx `unsigned short` DOES buy the xor/mov16 idiom, but flips the whole
   register triangle (first diff jumps to 0x4: target loads the param into EDX, ours
   EAX). cpermute annealer (4000 iters, all recipes) found no exact match, best
   145/170. The idiom and the allocation are coupled through 9.5b's allocator and can
   only be won together, which no source spelling reaches. Same encoding-tie wall as
   the g_dir_d* siblings (compass_tile_probe, 0x2d5b8). Not source-reachable. */
extern short g_dir_dx[];
extern short g_dir_dy[];
extern short g_aim_x;
extern short g_aim_y;
extern int FUN_0004d221(int, int);

int FUN_00026bc8(unsigned short param_1, int param_2, int param_3)
{
    int r;
    unsigned idx;
    short s2 = (short)param_2;
    short s3 = (short)param_3;

    if ((unsigned)(param_1 * param_1)
        <= (unsigned)(s2 * s2 + s3 * s3)) {
        r = FUN_0004d221(s2, s3);
        idx = (unsigned short)r;
        g_aim_x += (g_dir_dx[idx] * param_1) >> 8;
        g_aim_y += (g_dir_dy[idx] * param_1) >> 8;
        return r;
    }
    g_aim_x += param_2;
    g_aim_y += param_3;
    return -1;
}
