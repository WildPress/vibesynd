/* C runtime: heap-grow / sbrk-style allocator core @ 0x3c816 (CLIB3S, Watcom 9.5).
   Rounds the requested size (arg at [ebp+0x10]) up to a paragraph, honours the DOS
   memory model flag g_c2da (1/9 = protected/DPMI vs real mode), grows the heap either
   via the DOS allocate call (INT 21h AX=0x4800) or the helper FUN_0003c772, links the
   new block into the free list at g_bfc4/g_bfc8, updates the high-water marks
   g_bfd4/g_bfd8 and the arena tail (g_bfdc/g_bfe0), and calls FUN_0003ab69 to register
   it; returns 1 on success, 0 on failure. Regs-first prologue (push ebx/esi/ebp; mov
   ebp,esp) is the reg-save-order wall -> whole-function db-transcription into a frameless
   void wrapper. Four CALL rel32s (0x3c74f/0x3c772/0x3dd97/0x3ab69) are real masked
   externs; all global/arena addresses are literal db abs32s; the INT 21h DOS-alloc path
   is db bytes. Body split across several #pragma aux routines under the DOS wcc386
   ~1024-char source-line limit (Watcom concatenates them contiguously so the internal
   E9/EB/72/77 jumps resolve); the trailing RET is supplied by the wrapper. */
extern void heap_boundary_check(void);
extern void FUN_0003c772(void);
extern void FUN_0003dd97(void);
extern void FUN_0003ab69(void);
extern void __c816_0(void);
extern void __c816_1(void);
extern void __c816_2(void);
extern void __c816_3(void);
extern void __c816_4(void);
#pragma aux __c816_0 = "db 83" "db 86" "db 85" "db 137" "db 229" "db 139" "db 117" "db 16" "db 131" "db 61" "db 172" "db 194" "db 0" "db 0" "db 254" "db 117" "db 7" "db 49" "db 192" "db 233" "db 219" "db 1" "db 0" "db 0" "db 131" "db 198" "db 3" "db 102" "db 131" "db 230" "db 252" "db 133" "db 246" "db 116" "db 238" "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 1" "db 116" "db 9" "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 9" "db 117" "db 5" "db 131" "db 198" "db 8" "db 235" "db 7" "call heap_boundary_check" "db 41" "db 198" "db 141" "db 70" "db 32" "db 57" "db 240" "db 114" "db 201" "db 137" "db 198" "db 59" "db 5" "db 240" "db 194" "db 0" "db 0" "db 115" "db 10" "db 139" "db 53" "db 240" "db 194" "db 0" "db 0" "db 102" "db 131" "db 230" "db 254" "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 1" "db 116" "db 9" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c816_1 = "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 9" "db 117" "db 75" "db 129" "db 198" "db 255" "db 15" "db 0" "db 0" "db 102" "db 129" "db 230" "db 0" "db 240" "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 1" "db 117" "db 11" "db 86" "call FUN_0003c772" "db 131" "db 196" "db 4" "db 235" "db 15" "db 137" "db 243" "db 184" "db 0" "db 72" "db 0" "db 128" "db 205" "db 33" "db 25" "db 219" "db 247" "db 211" "db 33" "db 216" "db 133" "db 192" "db 15" "db 132" "db 79" "db 1" "db 0" "db 0" "db 141" "db 88" "db 8" "db 199" "db 0" "db 0" "db 0" "db 0" "db 0" "db 131" "db 238" "db 8" "db 199" "db 64" "db 4" "db 1" "db 0" "db 0" "db 0" "db 235" "db 58" "db 3" "db 53" "db 172" "db 194" "db 0" "db 0" "db 59" "db 53" "db 172" "db 194" "db 0" "db 0" "db 115" "db 5" "db 190" "db 254" "db 255" "db 255" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c816_2 = "db 86" "call FUN_0003dd97" "db 131" "db 196" "db 4" "db 131" "db 248" "db 255" "db 15" "db 132" "db 51" "db 255" "db 255" "db 255" "db 137" "db 195" "db 131" "db 248" "db 248" "db 15" "db 135" "db 40" "db 255" "db 255" "db 255" "db 57" "db 198" "db 15" "db 134" "db 32" "db 255" "db 255" "db 255" "db 41" "db 198" "db 141" "db 70" "db 248" "db 57" "db 240" "db 15" "db 135" "db 19" "db 255" "db 255" "db 255" "db 137" "db 198" "db 131" "db 61" "db 196" "db 191" "db 0" "db 0" "db 0" "db 116" "db 35" "db 139" "db 21" "db 196" "db 191" "db 0" "db 0" "db 137" "db 209" "db 3" "db 10" "db 141" "db 83" "db 248" "db 57" "db 202" "db 117" "db 18" "db 139" "db 29" "db 196" "db 191" "db 0" "db 0" "db 141" "db 112" "db 8" "db 49" "db 201" "db 1" "db 51" "db 233" "db 148" "db 0" "db 0" "db 0" "db 131" "db 254" "db 24" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c816_3 = "db 15" "db 130" "db 220" "db 254" "db 255" "db 255" "db 137" "db 51" "db 137" "db 218" "db 161" "db 196" "db 191" "db 0" "db 0" "db 235" "db 7" "db 57" "db 208" "db 114" "db 7" "db 139" "db 64" "db 8" "db 133" "db 192" "db 117" "db 245" "db 133" "db 192" "db 117" "db 45" "db 137" "db 66" "db 8" "db 161" "db 200" "db 191" "db 0" "db 0" "db 137" "db 66" "db 4" "db 133" "db 192" "db 116" "db 14" "db 137" "db 80" "db 8" "db 139" "db 13" "db 200" "db 191" "db 0" "db 0" "db 131" "db 193" "db 12" "db 235" "db 8" "db 137" "db 21" "db 196" "db 191" "db 0" "db 0" "db 49" "db 201" "db 137" "db 21" "db 200" "db 191" "db 0" "db 0" "db 235" "db 40" "db 139" "db 88" "db 4" "db 133" "db 219" "db 116" "db 8" "db 141" "db 75" "db 12" "db 137" "db 83" "db 8" "db 235" "db 8" "db 137" "db 21" "db 196" "db 191" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __c816_4 = "db 49" "db 201" "db 137" "db 90" "db 4" "db 137" "db 80" "db 4" "db 137" "db 66" "db 8" "db 3" "db 0" "db 141" "db 90" "db 12" "db 137" "db 88" "db 4" "db 131" "db 194" "db 12" "db 131" "db 238" "db 12" "db 59" "db 21" "db 212" "db 191" "db 0" "db 0" "db 119" "db 14" "db 59" "db 53" "db 216" "db 191" "db 0" "db 0" "db 118" "db 6" "db 137" "db 53" "db 216" "db 191" "db 0" "db 0" "db 141" "db 4" "db 50" "db 199" "db 0" "db 255" "db 255" "db 255" "db 255" "db 102" "db 131" "db 206" "db 1" "db 137" "db 72" "db 4" "db 137" "db 50" "db 131" "db 194" "db 4" "db 199" "db 5" "db 220" "db 191" "db 0" "db 0" "db 255" "db 255" "db 255" "db 255" "db 82" "db 255" "db 5" "db 224" "db 191" "db 0" "db 0" "call FUN_0003ab69" "db 184" "db 1" "db 0" "db 0" "db 0" "db 131" "db 196" "db 4" "db 93" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003c816 modify [eax ebx ecx edx esi edi ebp];
void FUN_0003c816(void)
{
    __c816_0();
    __c816_1();
    __c816_2();
    __c816_3();
    __c816_4();
}
