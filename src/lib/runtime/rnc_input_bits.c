/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Bitstream bit-extractor @ 0x3a3c6.
   Leaf. Reads CL (=AL param) bits out of the little-endian bit buffer held in globals
   g_bfbe (word, high half AX), g_bfbc (word, low/current half BX) and g_bfc1 (byte,
   bits still available CH), advancing the compressed-data pointer ESI by 2 and
   refilling AX from *ESI whenever the request crosses the current word boundary
   (SUB CH,CL borrow => refill path). Writes the updated buffer/counter back to the
   three globals and leaves the extracted bits in AX. ESI is a live in/out register
   shared with the callers (0x3a383, 0x3a449) -- deliberately NOT preserved, so it is
   placed in the pragma modify set to suppress a wrapper save.
   Body db-transcribed (minus the trailing RET, which the frameless void wrapper
   supplies). Split into two #pragma aux routines because the full 129-item db line
   exceeds the DOS wcc386 source-line limit; the JNC and its target both live in part1
   so the split point (offset 0x57) has no jump crossing it. */
extern void __getbits_a(unsigned char n);
extern void __getbits_b(void);
#pragma aux __getbits_a = "db 102" "db 81" "db 138" "db 200" "db 102" "db 161" "db 190" "db 191" "db 0" "db 0" "db 102" "db 139" "db 29" "db 188" "db 191" "db 0" "db 0" "db 138" "db 45" "db 193" "db 191" "db 0" "db 0" "db 102" "db 186" "db 1" "db 0" "db 102" "db 211" "db 226" "db 102" "db 74" "db 102" "db 35" "db 211" "db 102" "db 82" "db 42" "db 233" "db 115" "db 42" "db 2" "db 233" "db 134" "db 205" "db 102" "db 186" "db 1" "db 0" "db 102" "db 211" "db 226" "db 102" "db 74" "db 102" "db 35" "db 208" "db 102" "db 211" "db 202" "db 102" "db 211" "db 232" "db 102" "db 211" "db 235" "db 102" "db 11" "db 218" "db 131" "db 198" "db 2" "db 102" "db 139" "db 6" "db 134" "db 205" "db 42" "db 205" "db 181" "db 16" "db 42" "db 233" "db 102" "db 186" "db 1" "db 0" parm [al] modify exact [eax ebx ecx edx esi];
#pragma aux __getbits_b = "db 102" "db 211" "db 226" "db 102" "db 74" "db 102" "db 35" "db 208" "db 102" "db 211" "db 202" "db 102" "db 211" "db 232" "db 102" "db 211" "db 235" "db 102" "db 11" "db 218" "db 102" "db 163" "db 190" "db 191" "db 0" "db 0" "db 102" "db 137" "db 29" "db 188" "db 191" "db 0" "db 0" "db 136" "db 45" "db 193" "db 191" "db 0" "db 0" "db 102" "db 88" "db 102" "db 89" parm [] modify exact [eax ebx ecx edx esi];
/* Declare that rnc_input_bits itself clobbers ebx/esi (hand-asm convention, no save) so
   the wrapper does not add a spurious push/pop of the callee-saved regs. */
#pragma aux rnc_input_bits modify [eax ebx ecx edx esi];
void rnc_input_bits(unsigned char n)
{
    __getbits_a(n);
    __getbits_b();
}
