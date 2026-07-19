/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). RLE/sprite-stream decoder @ 0x3a10c.
   Framed hand-asm sibling of 0x3a033 (its own PUSH EBP; MOV EBP,ESP; ADD ESP,-4 prologue,
   LEAVE/RET epilogue), a simpler byte-oriented variant. Same header read from
   g_bdd8/g_bdda (dims into [ebp-2]/[ebp-4]) and g_bddc (height-6), same setup call
   0x3a7c4(g_bdd0, g_back_buf, height-6), then walks the control stream at g_back_buf into surface
   g_screen_buf: per row, the signed control byte DL selects add-run vs subtract-run of the
   accumulator CX (sign-extended via MOV AH,0xff / SUB AX,AX) and drives two byte-copy
   directions (INC/DEC ESI), decrementing the row counter BX ([ebp-4]) until zero.
   All GP regs + EBP are hand-asm scratch (EBP balanced by the body's own PUSH/LEAVE),
   declared in the wrapper modify set so no compiler frame/save is added. Body
   db-transcribed minus the trailing RET (frameless void wrapper supplies it), split into
   two #pragma aux routines to stay under the DOS wcc386 source-line limit (Watcom
   concatenates them contiguously, preserving internal jumps). CALL 0x3a7c4 is a real call
   to the extern symbol; match_reloc masks the reloc. */
extern void read(void);
extern void __rle2_a(void);
extern void __rle2_b(void);
#pragma aux __rle2_a = "db 85" "db 139" "db 236" "db 131" "db 196" "db 252" "db 102" "db 161" "db 216" "db 189" "db 0" "db 0" "db 102" "db 137" "db 69" "db 254" "db 102" "db 161" "db 218" "db 189" "db 0" "db 0" "db 102" "db 137" "db 69" "db 252" "db 15" "db 183" "db 5" "db 220" "db 189" "db 0" "db 0" "db 131" "db 232" "db 6" "db 80" "db 255" "db 53" "db 112" "db 83" "db 0" "db 0" "db 255" "db 53" "db 208" "db 189" "db 0" "db 0" "call read" "db 131" "db 196" "db 12" "db 139" "db 53" "db 104" "db 83" "db 0" "db 0" "db 139" "db 61" "db 112" "db 83" "db 0" "db 0" "db 102" "db 139" "db 93" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __rle2_b = "db 252" "db 71" "db 102" "db 139" "db 77" "db 254" "db 138" "db 23" "db 71" "db 128" "db 250" "db 0" "db 127" "db 9" "db 180" "db 255" "db 138" "db 194" "db 102" "db 3" "db 200" "db 235" "db 8" "db 102" "db 43" "db 192" "db 138" "db 194" "db 102" "db 43" "db 200" "db 128" "db 250" "db 0" "db 127" "db 12" "db 138" "db 55" "db 71" "db 136" "db 54" "db 70" "db 254" "db 194" "db 117" "db 246" "db 235" "db 10" "db 138" "db 55" "db 71" "db 136" "db 54" "db 70" "db 254" "db 202" "db 117" "db 249" "db 102" "db 131" "db 249" "db 0" "db 117" "db 198" "db 102" "db 75" "db 117" "db 189" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux rle_blit_byte modify [eax ebx ecx edx esi edi ebp];
void rle_blit_byte(void)
{
    __rle2_a();
    __rle2_b();
}
