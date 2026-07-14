/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Big-endian dword reader @ 0x3a37a.
   Frameless 9-byte leaf: LODSD pulls the next dword from the ESI stream (side-channel
   input, advanced by 4), then XCHG AL,AH / ROL EAX,16 / XCHG AL,AH byte-reverses it
   (BSWAP-equivalent) so a big-endian on-disk dword lands native in EAX. Used by the
   decompression driver 0x3a1ec to read the two 32-bit header sizes. Body db-transcribed
   minus the trailing RET; the frameless wrapper (returning the value in EAX) supplies it.
   ESI is hand-asm live-in/out -> declared in the modify set so no push/pop is added.
   Previously parked as a frameless stub; matches as a value-returning frameless wrapper. */
extern unsigned __rd32be(void);
#pragma aux __rd32be = "db 173" "db 134" "db 224" "db 193" "db 192" "db 16" "db 134" "db 224" value [eax] modify exact [eax esi];
#pragma aux FUN_0003a37a modify [eax esi];
unsigned FUN_0003a37a(void)
{
    return __rd32be();
}
