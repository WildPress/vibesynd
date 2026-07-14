/* 0x254a8 -- keyboard hook installer. Zeroes the 0x80-byte key-state table at
 * 0xe284 (short index, arr[i++]=0 do-while, loop-aligned), saves the old INT 9
 * vector (far ptr DX:EAX from d_getvec 0x3b239) into the 6-byte far-ptr global
 * at 0xdf0e, then d_setvec(9, handler 0x17cb0). The target's `push cs` is the
 * near->far conversion on an __interrupt __far handler.
 * Recipe: -4s -oneatx -zp8 -s -zq
 */
extern char g_e284[];
extern void (__interrupt __far *g_df0e)(void);
extern void (__interrupt __far *FUN_0003b239(int intno))(void);
extern void FUN_0003b273(int vec, void (__interrupt __far *fn)(void));
extern void __interrupt __far FUN_00017cb0(void);

void keyboard_hook_install(void)
{
    short i = 0;

    do {
        g_e284[i++] = 0;
    } while (i < 0x80);
    g_df0e = FUN_0003b239(9);
    FUN_0003b273(9, FUN_00017cb0);
}
