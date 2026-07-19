/* C runtime: fopen mode-string parser @ 0x3b739 (CLIB3S). Decodes r/w/a/+/b/t to stream flag bits (tolower, 0xc289 default-mode); regs-first prologue -> full db-transcription.
   Body db-transcribed (frameless wrapper; modify[] suppresses the wrapper frame).
   External calls are real masked relocs; abs data refs are literal bytes. */
extern void tolower();
extern void errno_ptr();
extern void __db_0003b739_0(void);
#pragma aux __db_0003b739_0 = "db 83" "db 86" "db 85" "db 137" "db 229" "db 139" "db 117" "db 16" "db 15" "db 182" "db 6" "db 80" "db 49" "db 219" "call tolower" "db 131" "db 196" "db 4" "db 136" "db 193" "db 60" "db 114" "db 116" "db 26" "db 60" "db 119" "db 116" "db 22" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b739_1(void);
#pragma aux __db_0003b739_1 = "db 60" "db 97" "db 116" "db 18" "call errno_ptr" "db 199" "db 0" "db 9" "db 0" "db 0" "db 0" "db 49" "db 192" "db 233" "db 117" "db 0" "db 0" "db 0" "db 137" "db 216" "db 12" "db 3" "db 128" "db 126" "db 1" "db 43" "db 117" "db 32" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b739_2(void);
#pragma aux __db_0003b739_2 = "db 137" "db 195" "db 12" "db 64" "db 128" "db 126" "db 2" "db 98" "db 116" "db 18" "db 128" "db 126" "db 2" "db 116" "db 116" "db 66" "db 129" "db 61" "db 137" "db 194" "db 0" "db 0" "db 0" "db 2" "db 0" "db 0" "db 117" "db 54" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b739_3(void);
#pragma aux __db_0003b739_3 = "db 137" "db 195" "db 235" "db 50" "db 137" "db 218" "db 128" "db 202" "db 64" "db 128" "db 126" "db 1" "db 98" "db 117" "db 13" "db 137" "db 211" "db 128" "db 126" "db 2" "db 43" "db 117" "db 31" "db 128" "db 202" "db 3" "db 235" "db 24" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b739_4(void);
#pragma aux __db_0003b739_4 = "db 128" "db 126" "db 1" "db 116" "db 117" "db 6" "db 128" "db 126" "db 2" "db 43" "db 235" "db 214" "db 129" "db 61" "db 137" "db 194" "db 0" "db 0" "db 0" "db 2" "db 0" "db 0" "db 117" "db 2" "db 137" "db 211" "db 128" "db 249" parm [] modify exact [eax ebx ecx edx esi edi];
extern int __db_0003b739_5(void);
#pragma aux __db_0003b739_5 = "db 119" "db 117" "db 5" "db 128" "db 203" "db 2" "db 235" "db 13" "db 128" "db 249" "db 97" "db 117" "db 5" "db 128" "db 203" "db 130" "db 235" "db 3" "db 128" "db 203" "db 1" "db 137" "db 216" "db 93" "db 94" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
#pragma aux parse_open_mode modify [eax ebx ecx edx esi edi];
int parse_open_mode(char *mode)
{
    __db_0003b739_0();
    __db_0003b739_1();
    __db_0003b739_2();
    __db_0003b739_3();
    __db_0003b739_4();
    return __db_0003b739_5();
}
