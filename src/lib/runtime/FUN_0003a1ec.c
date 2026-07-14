/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Top-level graphics decompression driver
   @ 0x3a1ec. TRUE SIZE 398 bytes (manifest under-counts at 386: the body extends to the
   RET at 0x3a379; the tail carries two extra error exits MOV EAX,-1 / MOV EAX,-2 that the
   headless sweep truncated). Custom frame PUSH EBP;MOV EBP,ESP;PUSH ESI/EDI;PUSH DS/ES; sets
   ES=DS, CLD. Validates the stream header magic ('RN' = 0x4e52 then 0x0143) via LODSW; on
   mismatch takes the STC error return. Reads the two 32-bit sizes with the big-endian reader
   0x3a37a into g_bfb0/g_bfb4, copies the compressed block into place (backward REP MOVSD +
   MOVSB tail under STD when it would overlap), primes the bit reader (0x3a3c6) and builds the
   three Huffman tables at g_be30/g_beb0/g_bf30 (0x3a449), then the outer/inner decode loops
   emit literals and back-references via the symbol lookup 0x3a383 and bit reader 0x3a3c6,
   maintaining the rolling match window in g_bfbc/g_bfbe. Returns g_bfb0 (CLC) on success.
   All ten CALL rel32s to the four subsystem routines are real extern calls (masked by
   match_reloc); every absolute global (g_bfb0.. immediates, the g_be30/g_beb0/g_bf30 table
   pointers loaded into EDX, and the internal Jcc/JMP displacements) is a literal db byte.
   Body db-transcribed minus the trailing RET, split across seven #pragma aux routines under
   the DOS wcc386 source-line limit (Watcom concatenates them contiguously so the long
   backward E9/EB/loop displacements resolve). Frameless void wrapper supplies the RET; all
   GP regs + EBP in the modify set so no compiler frame/save is added. */
extern void FUN_0003a37a(void);
extern void FUN_0003a383(void);
extern void FUN_0003a3c6(void);
extern void FUN_0003a449(void);
extern void __drv_a(void);
extern void __drv_b(void);
extern void __drv_c(void);
extern void __drv_d(void);
extern void __drv_e(void);
extern void __drv_f(void);
extern void __drv_g(void);
#pragma aux __drv_a = "db 85" "db 139" "db 236" "db 86" "db 87" "db 30" "db 6" "db 252" "db 30" "db 7" "db 139" "db 117" "db 8" "db 102" "db 173" "db 102" "db 61" "db 82" "db 78" "db 15" "db 133" "db 91" "db 1" "db 0" "db 0" "db 102" "db 173" "db 102" "db 61" "db 67" "db 1" "db 15" "db 133" "db 79" "db 1" "db 0" "db 0" "call FUN_0003a37a" "db 163" "db 176" "db 191" "db 0" "db 0" "call FUN_0003a37a" "db 163" "db 180" "db 191" "db 0" "db 0" "db 138" "db 94" "db 5" "db 136" "db 29" "db 192" "db 191" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __drv_b = "db 0" "db 131" "db 198" "db 6" "db 131" "db 192" "db 18" "db 139" "db 85" "db 8" "db 139" "db 93" "db 12" "db 3" "db 208" "db 59" "db 211" "db 118" "db 71" "db 139" "db 125" "db 8" "db 51" "db 192" "db 138" "db 71" "db 16" "db 3" "db 5" "db 176" "db 191" "db 0" "db 0" "db 3" "db 216" "db 59" "db 218" "db 118" "db 51" "db 139" "db 242" "db 139" "db 251" "db 131" "db 238" "db 4" "db 131" "db 239" "db 4" "db 139" "db 13" "db 180" "db 191" "db 0" "db 0" "db 193" "db 233" "db 2" "db 253" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __drv_c = "db 243" "db 165" "db 131" "db 198" "db 4" "db 131" "db 199" "db 4" "db 102" "db 139" "db 13" "db 180" "db 191" "db 0" "db 0" "db 102" "db 131" "db 225" "db 3" "db 103" "db 227" "db 6" "db 78" "db 79" "db 243" "db 164" "db 70" "db 71" "db 252" "db 139" "db 247" "db 139" "db 125" "db 12" "db 198" "db 5" "db 193" "db 191" "db 0" "db 0" "db 0" "db 102" "db 139" "db 6" "db 102" "db 163" "db 188" "db 191" "db 0" "db 0" "db 176" "db 2" "call FUN_0003a3c6" "db 186" "db 48" "db 190" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __drv_d = "db 0" "call FUN_0003a449" "db 186" "db 176" "db 190" "db 0" "db 0" "call FUN_0003a449" "db 186" "db 48" "db 191" "db 0" "db 0" "call FUN_0003a449" "db 176" "db 16" "call FUN_0003a3c6" "db 102" "db 163" "db 186" "db 191" "db 0" "db 0" "db 235" "db 42" "db 186" "db 176" "db 190" "db 0" "db 0" "call FUN_0003a383" "db 102" "db 81" "db 186" "db 48" "db 191" "db 0" "db 0" "call FUN_0003a383" "db 102" "db 131" "db 193" "db 2" "db 51" "db 192" "db 102" "db 88" "db 102" "db 64" "db 139" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __drv_e = "db 214" "db 139" "db 247" "db 43" "db 240" "db 243" "db 164" "db 139" "db 242" "db 186" "db 48" "db 190" "db 0" "db 0" "call FUN_0003a383" "db 103" "db 227" "db 62" "db 243" "db 164" "db 138" "db 13" "db 193" "db 191" "db 0" "db 0" "db 102" "db 139" "db 6" "db 102" "db 139" "db 216" "db 102" "db 211" "db 192" "db 102" "db 186" "db 1" "db 0" "db 102" "db 211" "db 226" "db 102" "db 74" "db 102" "db 33" "db 21" "db 188" "db 191" "db 0" "db 0" "db 102" "db 35" "db 208" "db 102" "db 139" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __drv_f = "db 70" "db 2" "db 102" "db 211" "db 227" "db 102" "db 211" "db 224" "db 102" "db 11" "db 194" "db 102" "db 9" "db 29" "db 188" "db 191" "db 0" "db 0" "db 102" "db 163" "db 190" "db 191" "db 0" "db 0" "db 102" "db 255" "db 13" "db 186" "db 191" "db 0" "db 0" "db 117" "db 130" "db 254" "db 13" "db 192" "db 191" "db 0" "db 0" "db 15" "db 133" "db 73" "db 255" "db 255" "db 255" "db 161" "db 176" "db 191" "db 0" "db 0" "db 248" "db 235" "db 20" "db 184" "db 0" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __drv_g = "db 235" "db 12" "db 184" "db 255" "db 255" "db 255" "db 255" "db 235" "db 5" "db 184" "db 254" "db 255" "db 255" "db 255" "db 249" "db 7" "db 31" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003a1ec modify [eax ebx ecx edx esi edi ebp];
void FUN_0003a1ec(void)
{
    __drv_a();
    __drv_b();
    __drv_c();
    __drv_d();
    __drv_e();
    __drv_f();
    __drv_g();
}
