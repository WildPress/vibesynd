/* C runtime: @ 0x3b9ca (CLIB3S). r=FUN_3ba80(a,b); FUN_3dae1(a); return r; regs-first prologue -> full db-transcription.
   Body db-transcribed (frameless wrapper; modify[] suppresses the wrapper frame).
   External calls are real masked relocs; abs data refs are literal bytes. */
extern void FUN_0003ba80();
extern void FUN_0003dae1();
extern void * __db_0003b9ca_0(void);
#pragma aux __db_0003b9ca_0 = "db 83" "db 85" "db 137" "db 229" "db 255" "db 117" "db 16" "db 255" "db 117" "db 12" "call FUN_0003ba80" "db 131" "db 196" "db 8" "db 255" "db 117" "db 12" "db 137" "db 195" "call FUN_0003dae1" "db 131" "db 196" "db 4" "db 137" "db 216" "db 93" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003b9ca modify [eax ebx ecx edx esi edi];
void * FUN_0003b9ca(void *a, int b)
{
    return __db_0003b9ca_0();
}
