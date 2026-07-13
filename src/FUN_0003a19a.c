/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Frame-rate throttle @ 0x3a19a.
   Leaf, frameless. If g_bdf8 (min interval, centiseconds) != 0, busy-waits on the
   DOS clock (INT 21h AH=2Ch: DH=sec, DL=1/100s) computing a 0..5999 centisecond-
   in-minute counter (sec*100 + hundredths), until (now - g_bdec) wrapped mod 6000
   >= g_bdf8, then stores now into g_bdec (last-tick). AX/BX/CX/DX are push/pop
   preserved, so the fn clobbers nothing (modify exact []). Reproduced by
   db-transcribing the whole body (minus the trailing RET) as a #pragma aux; the
   frameless void wrapper supplies only the RET. */
extern void __throttle_3a19a(void);
#pragma aux __throttle_3a19a = "db 131" "db 61" "db 248" "db 189" "db 0" "db 0" "db 0" "db 116" "db 72" "db 102" "db 80" "db 102" "db 83" "db 102" "db 81" "db 102" "db 82" "db 180" "db 44" "db 205" "db 33" "db 102" "db 184" "db 100" "db 0" "db 246" "db 230" "db 182" "db 0" "db 102" "db 3" "db 208" "db 102" "db 139" "db 202" "db 102" "db 43" "db 21" "db 236" "db 189" "db 0" "db 0" "db 102" "db 131" "db 250" "db 0" "db 116" "db 225" "db 127" "db 5" "db 102" "db 129" "db 194" "db 112" "db 23" "db 15" "db 183" "db 210" "db 59" "db 21" "db 248" "db 189" "db 0" "db 0" "db 124" "db 207" "db 102" "db 137" "db 13" "db 236" "db 189" "db 0" "db 0" "db 102" "db 90" "db 102" "db 89" "db 102" "db 91" "db 102" "db 88" parm [] modify exact [];
void FUN_0003a19a(void)
{
    __throttle_3a19a();
}
