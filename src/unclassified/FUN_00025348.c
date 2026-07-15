/* NEAR-MISS @ 0x25348 -- instruction stream byte-IDENTICAL (masked); only an alignment pad differs.
 *
 * target 43B: e8<28cc8> 8d80 00000000  8d52 00  8bdb  31c0 66a1<df12> 50 8b15<df0e> 52 6a09 e8<b273> 83c40c c3
 * ours   32B: e8<28cc8>                            31c0 66a1<df12> 50 8b15<df0e> 52 6a09 e8<b273> 83c40c c3
 *
 * The ONLY difference is an 11-byte NOP alignment pad the target inserts right after the
 * first CALL (lea eax,[eax+0]=8d80.., lea edx,[edx+0]=8d5200, mov ebx,ebx=8bdb) to align
 * the 0x25358 code that follows. Every other byte matches masked. This is Watcom's
 * code-alignment fill, which the default recipe never emits mid-function (only -ol emits
 * loop-alignment NOPs, and that also unrolls) -> code-alignment wall, not source-reachable
 * with the fixed recipe. Logic: call 0x28cc8; then FUN_0003b273(9, g_df0e, g_df12), whose
 * dispatch tail mirrors the matched 0x252d8 (df08/df0c -> df0e/df12, arg 8 -> 9). */
extern void build_two_buffers(void);
extern void FUN_0003b273(int a, int b, unsigned short c);
extern int g_df0e;
extern unsigned short g_df12;

void FUN_00025348(void)
{
    build_two_buffers();
    FUN_0003b273(9, g_df0e, g_df12);
}
