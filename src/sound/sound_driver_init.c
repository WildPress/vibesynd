/* frameless @ 0x35d08: driver/module load + init (int return, 1=ok 0=fail).
   Loads a 0x3d00-byte buffer via FUN_18158 -> g_11df0, reads a resource via
   FUN_17b48(buf,5,0) -> g_11df4, frees the buffer (FUN_3ab59), FUN_3954c(),
   registers the resource (FUN_398d7) -> handle g_snd_driver (-1 = fail+cleanup).
   FUN_39994(handle) returns a near ptr published as the DS-based far pointer
   g_11dfc (offset dword @0x11dfc, selector word @0x11e00); header word at +4
   must be 2. Params a/b/c default from header fields +0x14/+0x18/+0x10 when 0,
   then FUN_399b3(handle, c, a, bb, hdr[0x1c]) probes and FUN_399bd(same) commits.

   PARKED at 338/346, first diff 0x7. PROVEN HERE: param b is used via a
   register-resident local copy (bb -> ESI, its default writes SI not the slot;
   a/c defaults write their memory slots with full-EAX junk-upper stores).
   Residual walls: (1) ours ALWAYS promotes param a into a callee-saved reg at
   entry (esi/ebx/edi depending on spelling) while the target keeps a memory-
   homed until the push block � tried ushort/int/short typings, casts, volatile
   (worse), assignment-in-condition; nothing suppresses the promotion. (2) ours
   tail-MERGES the two plain `return 0` guards into one far block (jz rel32)
   where the target duplicates the 7-byte xor+pops+ret inline per site. Same
   scheduling/allocator family as the register-role walls. */
extern int alloc_init_with_errcode(int size, int flag);
extern int container_load(int buf, int kind, int flag);
extern void FUN_0003ab59(int buf);
extern void init_voice_tables(void);
extern int register_driver(int res);
extern void unload_all_drivers(int arg);
extern int voice_get_driver_obj(int handle);
extern int FUN_000399b3(int handle, int c, int a, int b, int d);
extern void start_voice(int handle, int c, int a, int b, int d);
extern int g_11df0;
extern int g_11df4;
extern int g_snd_driver;
extern unsigned char __far *g_11dfc;

int sound_driver_init(unsigned short a, unsigned short b, unsigned short c)
{
    unsigned short bb;

    bb = b;
    if ((g_11df0 = alloc_init_with_errcode(0x3d00, 0)) == 0)
        return 0;
    if ((g_11df4 = container_load(g_11df0, 5, 0)) == 0)
        return 0;
    FUN_0003ab59(g_11df0);
    init_voice_tables();
    g_snd_driver = register_driver(g_11df4);
    if (g_snd_driver == -1) {
        unload_all_drivers(0);
        return 0;
    }
    g_11dfc = (unsigned char __far *)voice_get_driver_obj(g_snd_driver);
    if (*(int __far *)(g_11dfc + 4) != 2) {
        unload_all_drivers(0);
        return 0;
    }
    if (a == 0)
        a = *(unsigned short __far *)(g_11dfc + 0x14);
    if (bb == 0)
        bb = *(unsigned short __far *)(g_11dfc + 0x18);
    if (c == 0)
        c = *(unsigned short __far *)(g_11dfc + 0x10);
    if (FUN_000399b3(g_snd_driver, c, a, bb,
                     *(int __far *)(g_11dfc + 0x1c)) == 0) {
        unload_all_drivers(0);
        return 0;
    }
    start_voice(g_snd_driver, c, a, bb, *(int __far *)(g_11dfc + 0x1c));
    return 1;
}
