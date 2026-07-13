/* FUN_00039e42 (cdecl/-4s, void). Chunk-reader loop for the music/sound file.
 * disasm: read a 2-byte count into g_bde0 and an 8-byte header into g_5370, then
 * for each remaining chunk read a 4-byte size (g_bddc) + 2-byte tag (g_bdd4) and
 * dispatch: 7->0x3a033, 4->0x39f69, 0xf->0x3a10c, else->0x39f49.
 *
 * NEAR-MISS (best 165/160, recipe -4s -os, first diff at 0x2c). The region is
 * compiled caller-saves-all: the target uses NO callee-saved reg saves and pushes
 * every global directly from memory (ff35 push-m32). Only -os reproduces this
 * (all other recipes home g_bdd0/g_bdd4 into EBX/ESI and add push/pop saves ->
 * 176B, diff at byte 0). Residual with -os: the while-condition. Target loads
 * `mov ax,[g_bde0]; cmp ax,0` then two short jumps (jne+jmp); -os emits the
 * compact `cmp word[g_bde0],0; jz <far>`. A comma-read `(n=g_bde0)!=0` gets the
 * mov-ax load but then reuses AX for the decrement (dec ax; mov [mem],ax) where
 * the target keeps an in-place `sub word[g_bde0],1`, and yields `test`/`cmp`
 * skew -- worse. -os(mem-cmp) vs -oneatx(load-into-callee-saved) is a codegen
 * tie-break; the target's load-into-AX-without-any-save form is not reachable in
 * isolation. Structure + all relocs are byte-correct. Parked (encoding wall). */
extern int   g_bdd0;
extern void *g_5370;
extern unsigned short g_bde0;
extern unsigned short g_bddc;
extern unsigned short g_bdd4;
extern int  FUN_0003a7c4(int, void *, int);
extern void FUN_0003a033(void);
extern void FUN_00039f69(void);
extern void FUN_0003a10c(void);
extern void FUN_00039f49(void);

void FUN_00039e42(void)
{
    FUN_0003a7c4(g_bdd0, &g_bde0, 2);
    FUN_0003a7c4(g_bdd0, g_5370, 8);
    while (g_bde0 != 0) {
        g_bde0--;
        FUN_0003a7c4(g_bdd0, &g_bddc, 4);
        FUN_0003a7c4(g_bdd0, &g_bdd4, 2);
        if (g_bdd4 == 7)
            FUN_0003a033();
        else if (g_bdd4 == 4)
            FUN_00039f69();
        else if (g_bdd4 == 0xf)
            FUN_0003a10c();
        else
            FUN_00039f49();
    }
}
