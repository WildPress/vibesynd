/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Huffman decode-table builder @ 0x3a449.
   Non-leaf (calls the bit-reader 0x3a3c6). Destination pointer arrives in EDX (saved via
   PUSH EDX at entry, later recovered as EDI=[ESI+0x10]). Reads a 5-bit count and, for each
   of up to 16 code lengths, reads a 4-bit symbol-count into a 0x10-byte stack scratch
   buffer, then walks bit-lengths AL=1..0x10 building, for every present length, the
   (mask, first-code, base) triples the lookup routine 0x3a383 consumes -- STOSW writes the
   masks/codes, and [EDI+0x3c] gets the canonical-Huffman running base. Frameless; keeps its
   own PUSH EDI/PUSH EDX/SUB ESP,0x10 frame in the body. All GP regs are hand-asm scratch
   (edi/edx/esi balanced internally) -> declared in the wrapper modify set to suppress
   compiler-added saves. Body db-transcribed minus the trailing RET (wrapper supplies it),
   split into two #pragma aux routines to stay under the DOS wcc386 source-line limit;
   Watcom concatenates consecutive inline pragmas contiguously so every internal relative
   jump (incl. the JCXZ spanning the split) resolves. The two CALL 0x3a3c6 rel32s are real
   calls to the extern symbol; match_reloc masks the relocs. */
extern void rnc_input_bits(unsigned char n);
extern void __hufbuild_a(void *dst);
extern void __hufbuild_b(void);
#pragma aux __hufbuild_a = "db 87" "db 82" "db 131" "db 236" "db 16" "db 139" "db 252" "db 176" "db 5" "call rnc_input_bits" "db 51" "db 201" "db 102" "db 139" "db 200" "db 103" "db 227" "db 125" "db 81" "db 176" "db 4" "call rnc_input_bits" "db 54" "db 136" "db 7" "db 71" "db 226" "db 243" "db 89" "db 86" "db 139" "db 244" "db 131" "db 198" "db 4" "db 54" "db 139" "db 126" "db 16" "db 176" "db 1" "db 102" "db 51" "db 219" "db 102" "db 186" "db 0" "db 128" "db 102" "db 81" "db 86" "db 54" "db 58" "db 6" "db 117" "db 67" "db 102" "db 80" "db 102" "db 83" "db 102" "db 81" "db 138" "db 200" "db 102" "db 184" "db 1" "db 0" parm [edx] modify exact [eax ebx ecx edx esi edi];
#pragma aux __hufbuild_b = "db 102" "db 211" "db 224" "db 102" "db 72" "db 102" "db 171" "db 138" "db 193" "db 177" "db 16" "db 42" "db 200" "db 102" "db 211" "db 235" "db 138" "db 200" "db 102" "db 51" "db 192" "db 102" "db 209" "db 219" "db 102" "db 209" "db 208" "db 226" "db 248" "db 102" "db 171" "db 139" "db 198" "db 43" "db 196" "db 102" "db 131" "db 232" "db 16" "db 54" "db 138" "db 38" "db 102" "db 137" "db 71" "db 60" "db 102" "db 89" "db 102" "db 91" "db 102" "db 88" "db 102" "db 3" "db 218" "db 70" "db 226" "db 181" "db 94" "db 102" "db 89" "db 102" "db 209" "db 234" "db 254" "db 192" "db 60" "db 17" "db 117" "db 166" "db 94" "db 131" "db 196" "db 16" "db 90" "db 95" parm [] modify exact [eax ebx ecx edx esi edi];
#pragma aux rnc_make_huffman parm [edx] modify [eax ebx ecx edx esi edi];
void rnc_make_huffman(void *dst)
{
    __hufbuild_a(dst);
    __hufbuild_b();
}
