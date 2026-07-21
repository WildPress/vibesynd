/* alloc_init_with_errcode @ 0x18158 - alloc/init sequence; sets g_3eec error code.
 *
 * NEAR-MISS (NOT matched). Structure/logic byte-correct; the residual is an encoding
 * tie: on each error path the target MATERIALISES the code in a register first
 * (mov ebp,3; xor eax,eax; mov [g_3eec],ebp) using a different reg per path (ebp/edi/
 * esi/ecx), while our Watcom stores the immediate directly (mov [g_3eec],3). Watcom
 * folds `err=N; g=err;` to a direct store for every C spelling, so the register-routed
 * store is not source-reachable here. Codegen tie-break wall. */
extern unsigned int g_3eec;
extern int file_open_read_close(int size);
extern int malloc(int h);            /* orig 0x18191 `call 0x3aa74`; FUN_0001aa74 was mis-transcribed */
extern int open(int size, int mode);
extern void FUN_0004aa59(int h);
extern int read(int a, int b, int c);
extern void FUN_0004b859(int h);
extern void close(int h);            /* orig 0x18209 `call 0x3a89d`; FUN_0003c89d was mis-transcribed */

#pragma aux alloc_init_with_errcode modify [eax ecx edx ebx];

int alloc_init_with_errcode(int size, int p2)
{
    int h1, h2, h3, r;

    g_3eec = 0;
    h1 = file_open_read_close(size);
    if (h1 == -1) {
        g_3eec = 3;
        return 0;
    }
    h2 = p2;
    if (h2 == 0)
        h2 = malloc(h1);
    if (h2 == 0) {
        g_3eec = 2;
        return 0;
    }
    h3 = open(size, 0x200);
    if (h3 == -1) {
        FUN_0004aa59(h2);
        g_3eec = 3;
        return 0;
    }
    r = read(h3, h2, h1);
    if (r != h1) {
        FUN_0004b859(h2);
        g_3eec = 5;
        return 0;
    }
    close(h3);
    return h2;
}
