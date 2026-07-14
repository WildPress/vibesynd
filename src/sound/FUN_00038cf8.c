/* 0x38cf8 -- XMIDI music-system init (AIL-style), sibling of 0x35d08.
   Loads driver file (a2) via FUN_18158, extracts resource kind 5, registers
   it (FUN_398d7 -> handle g_11e2c), header word +4 must be 3 (XMIDI driver).
   Params a/b/c (irq-ish trio) default from header +0x14/+0x18/+0x10 when 0;
   b is a register local copy bb (EDI at entry, default writes DI).
   FUN_399b3 probes, FUN_399bd commits. Then loads the music image (a1),
   builds "DATA/SAMPLE." + hdr suffix (+8), optional global timbre cache
   (FUN_39b73/39b7d), fopen(buf,"rb") @0x3b8f8, registers 8 sequences
   (FUN_39b5f -> g_11e0c[i]) with per-seq state allocs, and services timbre
   requests (FUN_39b87 -> bank/patch = w/256, w%256) from the sample file via
   FUN_38c28 + FUN_39b91. fclose @0x3b99e. Returns 1 ok / 0 fail.

   PARKED near-miss, 713/741 (28 bytes short). Same wall family as the parked
   sibling 0x35d08: (1) frame 0x78 vs ours 0x74 — the target promotes a param
   into a callee-saved reg early (`mov edi,[esp+0x98]` at entry) and carries one
   extra 4-byte local; ours defers the load; (2) the guard `return 0` tails —
   the target duplicates the short `xor eax; jmp/pops; ret` inline per site with
   a shared final tail (`jnz +7; xor eax; jmp end`), where ours tail-MERGES them
   into one far block (`jz rel32`), accounting for most of the 28-byte deficit.
   Param auto-promotion + return-tail-merge are allocator/cross-jumper internal
   (documented walls, cont.21/22); not source-reachable here. */
extern int FUN_00018158(int name, int flag);
extern int FUN_00017b48(int buf, int kind, int flag);
extern void FUN_0003ab59(void *buf);
extern int FUN_000398d7(int res);
extern unsigned char *FUN_00039994(int handle);
extern int FUN_000399b3(int handle, int c, int a, int b, int d);
extern void FUN_000399bd(int handle, int c, int a, int b, int d);
extern int FUN_00039b55(int handle);
extern int FUN_00039b5f(int handle, int image, unsigned seq, void *state, int ctl);
extern unsigned short FUN_00039b73(int handle);
extern void FUN_00039b7d(int handle, void *p, int n);
extern unsigned short FUN_00039b87(int handle, int seq);
extern void FUN_00039b91(int handle, int bank, int patch, void *p);
extern void *FUN_0003aa74(unsigned n);
extern void FUN_0003a900(char *dst, unsigned char *src);
extern void *FUN_0003b8f8(char *name, char *mode);
extern void FUN_0003b99e(void *f);
extern void *FUN_00038c28(void *f, int bank, int patch);

extern unsigned short g_11e30;
extern int g_11e2c;
extern int g_11e0c[8];

void *memcpy(void *dst, const void *src, unsigned len);
#pragma intrinsic(memcpy)

int FUN_00038cf8(int a1, int a2, unsigned short a, unsigned short b, unsigned short c)
{
    char buf[64];
    void *slots[8];
    int mem;
    int size;
    unsigned i;
    void *file;
    unsigned short bb;
    unsigned char *hdr;
    int drv, res;
    unsigned short w;
    void *p;
    int wv, q;

    bb = b;
    g_11e30 = 0;
    drv = FUN_00018158(a2, 0);
    if (drv == 0)
        return 0;
    res = FUN_00017b48(drv, 5, 0);
    if (res == 0)
        return 0;
    FUN_0003ab59((void *)drv);
    g_11e2c = FUN_000398d7(res);
    if (g_11e2c == -1)
        return 0;
    hdr = FUN_00039994(g_11e2c);
    if (*(int *)(hdr + 4) != 3)
        return 0;
    if (a == 0)
        a = *(unsigned short *)(hdr + 0x14);
    if (bb == 0)
        bb = *(unsigned short *)(hdr + 0x18);
    if (c == 0)
        c = *(unsigned short *)(hdr + 0x10);
    if (FUN_000399b3(g_11e2c, c, a, bb, *(int *)(hdr + 0x1c)) == 0)
        return 0;
    FUN_000399bd(g_11e2c, c, a, bb, *(int *)(hdr + 0x1c));
    size = FUN_00039b55(g_11e2c);
    mem = FUN_00018158(a1, 0);
    if (mem == 0)
        return 0;
    memcpy(buf, (char *)0x3d10, 13);
    FUN_0003a900(buf, hdr + 8);
    w = FUN_00039b73(g_11e2c);
    if (w != 0)
        FUN_00039b7d(g_11e2c, FUN_0003aa74(w), w);
    file = FUN_0003b8f8(buf, (char *)0x3d20);
    for (i = 0; i < 8; i++) {
        slots[i] = FUN_0003aa74(size);
        if ((g_11e0c[i] = FUN_00039b5f(g_11e2c, mem, i, slots[i], 0)) == -1) {
            FUN_0003ab59(slots[i]);
            break;
        }
        while ((w = FUN_00039b87(g_11e2c, g_11e0c[i])) != 0xffff) {
            wv = w;
            q = wv / 256;
            p = FUN_00038c28(file, (unsigned short)q, (unsigned short)(wv % 256));
            if (p == 0)
                return 0;
            FUN_00039b91(g_11e2c, (unsigned short)q, (unsigned short)(wv % 256), p);
            FUN_0003ab59(p);
        }
    }
    if (file != 0)
        FUN_0003b99e(file);
    return 1;
}
