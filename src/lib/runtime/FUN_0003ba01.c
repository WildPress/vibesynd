/* C runtime: temp filename builder @ 0x3ba01 (CLIB3S). Builds t<hex>_<hex>.tmp via nibble->hex 0x3b9ee; regs-first prologue -> full db-transcription.
   Body db-transcribed (frameless wrapper; modify[] suppresses the wrapper frame).
   External calls are real masked relocs; abs data refs are literal bytes. */
extern void FUN_0003db63();
extern void fclose();
extern void __db_0003ba01_0(void);
#pragma aux __db_0003ba01_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 131" "db 236" "db 4" "db 139" "db 125" "db 20" "call FUN_0003db63" "db 187" "db 4" "db 0" "db 0" "db 0" "db 137" "db 198" "db 137" "db 125" "db 252" "db 198" "db 7" "db 116" "db 1" "db 251" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003ba01_1(void);
#pragma aux __db_0003ba01_1 = "db 235" "db 20" "db 137" "db 240" "db 131" "db 224" "db 15" "db 80" "db 193" "db 238" "db 4" "call fclose" "db 131" "db 196" "db 4" "db 136" "db 3" "db 75" "db 59" "db 93" "db 252" "db 117" "db 231" "db 139" "db 69" "db 24" "db 193" "db 248" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003ba01_2(void);
#pragma aux __db_0003ba01_2 = "db 4" "db 131" "db 224" "db 15" "db 80" "db 198" "db 71" "db 5" "db 95" "call fclose" "db 136" "db 71" "db 6" "db 139" "db 69" "db 24" "db 131" "db 196" "db 4" "db 131" "db 224" "db 15" "db 80" "call fclose" "db 136" "db 71" "db 7" "db 198" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003ba01_3(void);
#pragma aux __db_0003ba01_3 = "db 71" "db 8" "db 46" "db 198" "db 71" "db 9" "db 116" "db 198" "db 71" "db 10" "db 109" "db 198" "db 71" "db 11" "db 112" "db 131" "db 196" "db 4" "db 198" "db 71" "db 12" "db 0" "db 201" "db 95" "db 94" "db 91" parm [] modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003ba01 modify [eax ebx ecx edx esi edi];
void FUN_0003ba01(char *buf, int a)
{
    __db_0003ba01_0();
    __db_0003ba01_1();
    __db_0003ba01_2();
    __db_0003ba01_3();
}
