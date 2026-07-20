/* BEHAVIOURALLY EQUIVALENT (verified 2026-07-21): whole-TU register-alloc/schedule tie. Ours
   emits a dead STORE `mov [buf+0xc],ebx` (buf[3]=param_1, immediately overwritten by
   buf[3]=(ushort)param_2 with no intervening read/call) and loads ebx(param_1) before esi(param_2);
   target folds the dead store into a dead READ `mov eax,ebx` and loads esi(param_2) first. Final
   memory image identical: buf[0]=0x101, buf[3]=(unsigned short)param_2. Same guard
   (param_1!=0 || (ushort)param_2!=0), same memset x2, same int386(0x31,buf,buf+7), same buf[13]
   test, same report_net_status(0x376c,0x2af,-1). Ours +1B (dead store vs dead read).

   PARKED near-miss (NOT matched, 103/130) -- guard + build two stack msg buffers + dispatch.
   Single int buf[14] reproduces the exact stack layout. WHOLE-TU register-alloc wall: the target
   promotes param_1 to a callee-saved reg with a dead `mov eax,ebx` READ artifact from its module
   compilation context; isolated compile emits a dead STORE + reversed param load order. Same
   fundamental class as 0x37818 (module-context, not source-reachable). */
/* dpmi_free_dos_mem @ 0x287e8 - guard + build two 0x1c-byte stack msg buffers, dispatch.
 * NEAR-MISS (103/130, first diff @0x6). Single 14-int array (msg=buf[0..6],
 * scratch=buf[7..13]) reproduces the target's stack layout exactly (memset
 * offsets, adb2 pushes, final cmp[esp+34] all match). The two dead buf[3]=param_1
 * stores force param_1 into a callee-saved reg (push esi) as in the target.
 * Remaining diff is a register-alloc/schedule wall: target keeps param_1 in EBX
 * with a dead READ (mov eax,ebx) and loads esi(param_2) before ebx(param_1);
 * our isolated compile emits a dead STORE + reversed param load order. */

extern void memset(void *dst, int val, int len);   /* memset */
extern void int386(int a, void *msg, void *scratch);
extern void report_net_status(int a, int b, int c);

void dpmi_free_dos_mem(int param_1, unsigned short param_2)
{
    int buf[14];

    if (param_1 != 0 || param_2 != 0) {
        memset(buf, 0, 0x1c);
        memset(buf + 7, 0, 0x1c);
        buf[0] = 0x101;
        buf[3] = param_1;
        buf[3] = param_1;
        buf[3] = param_2;
        int386(0x31, buf, buf + 7);
        if (buf[13] != 0)
            report_net_status(0x376c, 0x2af, -1);
    }
}
