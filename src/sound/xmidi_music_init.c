/* 0x38cf8 -- XMIDI music-system init (AIL-style), sibling of 0x35d08.
   Loads driver file (a2) via FUN_18158, extracts resource kind 5, registers
   it (FUN_398d7 -> handle g_seq_ctx), header word +4 must be 3 (XMIDI driver).
   Params a/b/c (irq-ish trio) default from header +0x14/+0x18/+0x10 when 0;
   b is a register local copy bb (EDI at entry, default writes DI).
   FUN_399b3 probes, FUN_399bd commits. Then loads the music image (a1),
   builds "DATA/SAMPLE." + hdr suffix (+8), optional global timbre cache
   (FUN_39b73/39b7d), fopen(buf,"rb") @0x3b8f8, registers 8 sequences
   (FUN_39b5f -> g_seq_state[i]) with per-seq state allocs, and services timbre
   requests (FUN_39b87 -> bank/patch = w/256, w%256) from the sample file via
   FUN_38c28 + FUN_39b91. fclose @0x3b99e. Returns 1 ok / 0 fail.

   NEAR-MISS, ours 698B vs target 741B, EDIT-DIST=334 (was 335).

   ONE section made byte-exact: the 13-byte "DATA/SAMPLE." copy. The target emits
   it UNROLLED (`movsd;movsd;movsd;movsb` = a5 a5 a5 a4, interleaved with the
   strcat arg-pushes). `memcpy(buf,0x3d10,13)` under `#pragma intrinsic` compiled
   instead to a `rep movs` loop (mov ecx,13; shr ecx,2; rep movsd; and cl,3; rep
   movsb) -- the WRONG shape. Copying via an aggregate assignment
   (`*(struct blk13*)buf = *(struct blk13*)0x3d10`) reproduces the target's exact
   unrolled movs; the original clearly used an array/struct copy, not memcpy. The
   aggregate metric barely moves (the register cascade + shorter length re-align
   downstream) but the section itself now matches.

   REMAINING walls (allocator/cross-jumper internal, confirmed not source-
   reachable): (1) frame 0x78 vs ours 0x74 -- the target SPILLS param `c` to a
   local stack slot [esp+0x74] and keeps `hdr` in EBP, whereas ours enregisters
   `c` in EBP and `hdr` in EBX; that one extra 4-byte spill shifts every [esp+N]
   downstream. Forcing the spill via a copy local (`q=c`) or reordering the
   drv/res/hdr declarations did NOT move it. (2) The guard `return 0` tails --
   the target duplicates the full `xor eax; add esp,0x78; pops; ret` inline per
   site; ours reuses eax==0 from the failing test and tail-MERGES to the shared
   success exit (`je end`), 28 bytes shorter. Same source (early `return 0;`) --
   the cross-jumper decision is not source-controllable. */
extern int alloc_init_with_errcode(int name, int flag);
extern int container_load(int buf, int kind, int flag);
extern void FUN_0003ab59(void *buf);
extern int register_driver(int res);
extern unsigned char *voice_get_driver_obj(int handle);
extern int FUN_000399b3(int handle, int c, int a, int b, int d);
extern void start_voice(int handle, int c, int a, int b, int d);
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
extern int g_seq_ctx;
extern int g_seq_state[8];

void *memcpy(void *dst, const void *src, unsigned len);
#pragma intrinsic(memcpy)

struct blk13 { char b[13]; };

int xmidi_music_init(int a1, int a2, unsigned short a, unsigned short b, unsigned short c)
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
    drv = alloc_init_with_errcode(a2, 0);
    if (drv == 0)
        return 0;
    res = container_load(drv, 5, 0);
    if (res == 0)
        return 0;
    FUN_0003ab59((void *)drv);
    g_seq_ctx = register_driver(res);
    if (g_seq_ctx == -1)
        return 0;
    hdr = voice_get_driver_obj(g_seq_ctx);
    if (*(int *)(hdr + 4) != 3)
        return 0;
    if (a == 0)
        a = *(unsigned short *)(hdr + 0x14);
    if (bb == 0)
        bb = *(unsigned short *)(hdr + 0x18);
    if (c == 0)
        c = *(unsigned short *)(hdr + 0x10);
    if (FUN_000399b3(g_seq_ctx, c, a, bb, *(int *)(hdr + 0x1c)) == 0)
        return 0;
    start_voice(g_seq_ctx, c, a, bb, *(int *)(hdr + 0x1c));
    size = FUN_00039b55(g_seq_ctx);
    mem = alloc_init_with_errcode(a1, 0);
    if (mem == 0)
        return 0;
    *(struct blk13 *)buf = *(struct blk13 *)0x3d10;
    FUN_0003a900(buf, hdr + 8);
    w = FUN_00039b73(g_seq_ctx);
    if (w != 0)
        FUN_00039b7d(g_seq_ctx, FUN_0003aa74(w), w);
    file = FUN_0003b8f8(buf, (char *)0x3d20);
    for (i = 0; i < 8; i++) {
        slots[i] = FUN_0003aa74(size);
        if ((g_seq_state[i] = FUN_00039b5f(g_seq_ctx, mem, i, slots[i], 0)) == -1) {
            FUN_0003ab59(slots[i]);
            break;
        }
        while ((w = FUN_00039b87(g_seq_ctx, g_seq_state[i])) != 0xffff) {
            wv = w;
            q = wv / 256;
            p = FUN_00038c28(file, (unsigned short)q, (unsigned short)(wv % 256));
            if (p == 0)
                return 0;
            FUN_00039b91(g_seq_ctx, (unsigned short)q, (unsigned short)(wv % 256), p);
            FUN_0003ab59(p);
        }
    }
    if (file != 0)
        FUN_0003b99e(file);
    return 1;
}
