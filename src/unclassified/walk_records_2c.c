/* frameless(sub-esp) @ 0x183e8: walk records (stride 0x2c) while field +0x1c != 0.
   Each record's +0x1c is a pointer (obj); if obj[0] (val) != 0: test byte +0x28 & 1 ->
   build a small stack msg {[0]=0x101, +0xc=(u16)+0x2a} and call int386(0x31,&msg,
   scratch); else FUN_00018488(val, *(int*)+0x24). Then clear obj[0]=0. Top-guard + do/while
   on the +0x1c pointer field. Recipe: -4s -oneat -zp8 -s -zq (no -x, else the loop-back
   field access CSEs into EDI).

   NEAR-MISS best~89/106 (register-role wall). Structure/loop/buffers/no-hoist all correct.
   Target keeps the FIELD VALUE (val) in callee-saved ESI and the else-arg in EDI; because
   both callee-saved regs are occupied on the else path, Watcom's LICM finds no free register
   and emits the 0x101 store as an inline immediate (`mov dword[esp+0x1c],0x101`). Our Watcom
   9.5b instead assigns val/temp to VOLATILE registers (their live ranges never cross a call),
   which frees ESI so LICM hoists 0x101 into it. The only way found to occupy ESI across the
   loop (and thus suppress the hoist + get push-edi) is to keep the POINTER `obj` persistent
   (used in the clear obj[0]=0), which lands obj in ESI and val in ECX -- the exact inverse of
   the target's obj(transient EAX)/val(ESI) roles. No recipe (-oneatx/-oneat/-ot/-or/-oi/-oe/
   -ol/-oh/-os/-4r/-5s/-oaxt/plain), no C form (named/inline val, obj-persistent, param-direct
   vs cast, decl orderings, volatile store, val&0 live-extension, while/do-while/for), and no
   permuter run (8000 named + 8000 inline + 6000 obj, all ceiling ~87-90/106) flips the
   allocation. Same class as the parked 0x34048/0x34088/0x26e18/0x33fb8 register-role walls. */
extern void FUN_00018488(int a, int b);
extern void int386(int a, void *msg, void *scratch);

void walk_records_2c(int param_1)
{
    unsigned char *p = (unsigned char *)param_1;
    int msg[7];
    int scratch[7];
    int *obj;

    if (*(int *)(p + 0x1c) != 0) {
        do {
            obj = *(int **)(p + 0x1c);
            if (obj[0] != 0) {
                if (p[0x28] & 1) {
                    msg[0] = 0x101;
                    *(unsigned short *)((char *)msg + 0xc) = *(unsigned short *)(p + 0x2a);
                    int386(0x31, msg, scratch);
                } else {
                    FUN_00018488(obj[0], *(int *)(p + 0x24));
                }
                obj[0] = 0;
            }
            p += 0x2c;
        } while (*(int *)(p + 0x1c) != 0);
    }
}
