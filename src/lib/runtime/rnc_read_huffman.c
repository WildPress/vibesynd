/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Huffman symbol lookup @ 0x3a383.
   Non-leaf (calls the bit-reader 0x3a3c6 twice). Input: table pointer in EDX.
   XCHG ESI,EDX puts the table in ESI, then scans (mask_word, code_word) pairs with
   LODSW until (mask_word & g_bfbc) == code_word, reads the matched entry's length
   field at [ESI+0x3c], restores ESI, and consumes that many bits via 0x3a3c6; if the
   symbol is >= 2 bits it consumes one more and ORs in the top bit to rebuild the value
   in CX. ESI/EDX/EBX/ECX/EAX are hand-asm scratch (not preserved) -> declared in the
   wrapper's modify set. The two CALL 0x3a3c6 rel32s are emitted as real calls (extern
   symbol); match_reloc masks the reloc. Body db-transcribed minus the trailing RET
   (frameless void wrapper supplies it; the internal JC-to-RET lands on the wrapper RET). */
extern void rnc_input_bits(unsigned char n);
extern void __huflook(void *table);
#pragma aux __huflook = "db 135" "db 214" "db 102" "db 139" "db 13" "db 188" "db 191" "db 0" "db 0" "db 102" "db 173" "db 102" "db 139" "db 216" "db 102" "db 35" "db 217" "db 102" "db 173" "db 102" "db 59" "db 195" "db 117" "db 241" "db 102" "db 139" "db 78" "db 60" "db 135" "db 214" "db 138" "db 197" "call rnc_input_bits" "db 50" "db 237" "db 128" "db 249" "db 2" "db 114" "db 22" "db 254" "db 201" "db 138" "db 193" "call rnc_input_bits" "db 102" "db 187" "db 1" "db 0" "db 102" "db 211" "db 227" "db 102" "db 11" "db 195" "db 102" "db 139" "db 200" parm [edx] modify exact [eax ebx ecx edx esi];
#pragma aux rnc_read_huffman parm [edx] modify [eax ebx ecx edx esi];
void rnc_read_huffman(void *table)
{
    __huflook(table);
}
