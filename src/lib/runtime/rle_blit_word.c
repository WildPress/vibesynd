/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). RLE/sprite-stream decoder @ 0x3a033.
   Framed hand-asm (its own PUSH EBP; MOV EBP,ESP; ADD ESP,-0x10 prologue, LEAVE/RET
   epilogue). Reads the frame header from globals g_bdd8/g_bdda (dims) and g_bddc
   (height), calls the setup/unpack routine 0x3a7c4(g_bdd0, g_back_buf, height-6), then walks
   the control stream at g_back_buf decoding into the destination surface g_screen_buf: each word's
   AH top bits select skip (TEST AH,0x80/0x40 -> signed pixel/row advance via
   MOVSX/NEG/IMUL of the row stride) vs literal-run copy (byte count DL, sign decides two
   copy directions), storing 16-bit pixel pairs. Loops g_bdd8 (=[ebp-8]) scanlines.
   All GP regs + EBP are hand-asm scratch (EBP balanced by the body's own PUSH/LEAVE);
   declared in the wrapper modify set so no compiler frame/save is added. Body
   db-transcribed minus the trailing RET (frameless void wrapper supplies it), split into
   three #pragma aux routines to stay under the DOS wcc386 source-line limit (Watcom
   concatenates them contiguously, preserving every internal relative jump). CALL 0x3a7c4
   is a real call to the extern symbol; match_reloc masks the reloc. */
extern void read(void);
extern void __rle_a(void);
extern void __rle_b(void);
extern void __rle_c(void);
#pragma aux __rle_a = "db 85" "db 139" "db 236" "db 131" "db 196" "db 240" "db 15" "db 183" "db 5" "db 216" "db 189" "db 0" "db 0" "db 137" "db 69" "db 252" "db 102" "db 161" "db 218" "db 189" "db 0" "db 0" "db 102" "db 137" "db 69" "db 250" "db 15" "db 183" "db 5" "db 220" "db 189" "db 0" "db 0" "db 131" "db 232" "db 6" "db 80" "db 255" "db 53" "db 112" "db 83" "db 0" "db 0" "db 255" "db 53" "db 208" "db 189" "db 0" "db 0" "call read" "db 131" "db 196" "db 12" "db 139" "db 53" "db 104" "db 83" "db 0" "db 0" "db 139" "db 61" "db 112" "db 83" "db 0" "db 0" "db 102" "db 139" "db 7" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __rle_b = "db 102" "db 137" "db 69" "db 248" "db 131" "db 199" "db 2" "db 102" "db 139" "db 7" "db 131" "db 199" "db 2" "db 137" "db 117" "db 240" "db 246" "db 196" "db 128" "db 116" "db 29" "db 246" "db 196" "db 64" "db 117" "db 12" "db 86" "db 3" "db 117" "db 252" "db 131" "db 238" "db 1" "db 136" "db 6" "db 94" "db 235" "db 12" "db 15" "db 191" "db 192" "db 247" "db 216" "db 247" "db 109" "db 252" "db 3" "db 240" "db 235" "db 213" "db 102" "db 137" "db 69" "db 246" "db 102" "db 131" "db 248" "db 0" "db 116" "db 65" "db 138" "db 31" "db 71" "db 15" "db 182" "db 219" "db 3" "db 243" "db 138" "db 23" "db 71" "db 128" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __rle_c = "db 250" "db 0" "db 127" "db 26" "db 102" "db 139" "db 31" "db 131" "db 199" "db 2" "db 102" "db 137" "db 30" "db 131" "db 198" "db 2" "db 128" "db 194" "db 1" "db 117" "db 245" "db 102" "db 131" "db 109" "db 246" "db 1" "db 117" "db 216" "db 235" "db 23" "db 102" "db 139" "db 31" "db 131" "db 199" "db 2" "db 102" "db 137" "db 30" "db 131" "db 198" "db 2" "db 254" "db 202" "db 117" "db 240" "db 102" "db 131" "db 109" "db 246" "db 1" "db 117" "db 191" "db 139" "db 117" "db 240" "db 3" "db 117" "db 252" "db 102" "db 131" "db 109" "db 248" "db 1" "db 116" "db 5" "db 233" "db 120" "db 255" "db 255" "db 255" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux rle_blit_word modify [eax ebx ecx edx esi edi ebp];
void rle_blit_word(void)
{
    __rle_a();
    __rle_b();
    __rle_c();
}
