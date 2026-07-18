/* frameless @ 0x35f78: fire a sound-effect voice for slot `idx`.
   Guarded by two global enables (g_10b4a audio-on, g_537f sfx-on). Looks up the
   0x20-byte voice record g_5350[idx], then calls the low-level driver 0x39ae7 with
   the current DS selector plus the record's sample offset (+0x12) and length (+0xe),
   and finally kicks the mixer via 0x39b05. DS is read inline via a db-byte pragma
   (Watcom mini-asm rejects `mov edx,ds`); pinned to edx to match the original.

   PARKED near-miss (89/90). The ds-read pragma matches exactly. Two register tie-breaks
   remain, neither source-reachable (cpermute 2000 variants, no match): (1) the record
   address loads g_5350 into edx and shifts idx in eax, where the original copies idx to
   edx and loads g_5350 into eax (1 byte); (2) arg3 (rec+0xe) lands in edx, which getDS
   clobbers, so we push it early, where the original holds it in ebx across the ds read.
   Lifting arg3 to a local does force ebx but cascades the whole allocation (loses esi on
   the second call). Register-role/codegen-strategy family. */
extern char g_10b4a;
extern char g_537f;
extern unsigned char *g_5350;
extern int g_11dec;
extern void FUN_00039ae7(int h, int off, unsigned seg, int len, int flag);
extern void FUN_00039b05(int h);

extern unsigned getDS(void);
#pragma aux getDS = "db 0x8c" "db 0xda" value [edx] modify exact [edx];

void play_sound_slot(unsigned char idx)
{
    unsigned char *rec;
    if (g_10b4a == 0)
        return;
    if (g_537f == 0)
        return;
    rec = g_5350 + idx * 0x20;
    FUN_00039ae7(g_11dec, *(int *)(rec + 0x12), getDS() & 0xffff, *(int *)(rec + 0xe), -1);
    FUN_00039b05(g_11dec);
}
