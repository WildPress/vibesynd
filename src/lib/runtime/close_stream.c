/* C runtime: stream teardown / _fclose core @ 0x3ba80 (CLIB3S, Watcom 9.5). FILE* at
   [ebp+0x10], flag [ebp+0x14]. Handle field [EBX+0xc]==0 -> return -1. Flush (0x3d966 when
   [EBX+0xd]&0x10), fflush (0x3da03); on success lseek (0x3a93b) fd [EBX+0x10]; when flag set
   close (0x3a89d) fd; free malloc-d buffer [EBX+8] via 0x3ab59 when [EBX+0xc]&8; when
   [EBX+0xd]&8 tear down temp via 0x3ba01 + 0x3db69. Returns OR of sub-results. Full-body
   db-transcription incl the 7 calls (literal rel32s); trailing RET dropped. */
extern void __0003ba80_0(void);
#pragma aux __0003ba80_0 = "db 83" "db 86" "db 85" "db 137" "db 229" "db 131" "db 236" "db 16" "db 139" "db 93" "db 16" "db 131" "db 123" "db 12" "db 0" "db 117" "db 10" "db 184" "db 255" "db 255" "db 255" "db 255" "db 233" "db 127" "db 0" "db 0" "db 0" "db 49" "db 246" "db 246" "db 67" "db 13" "db 16" "db 116" "db 11" "db 83" "db 232" "db 189" "db 30" "db 0" "db 0" "db 131" "db 196" "db 4" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003ba80_1(void);
#pragma aux __0003ba80_1 = "db 137" "db 198" "db 83" "db 232" "db 79" "db 31" "db 0" "db 0" "db 131" "db 196" "db 4" "db 131" "db 248" "db 255" "db 116" "db 14" "db 106" "db 0" "db 80" "db 255" "db 115" "db 16" "db 232" "db 116" "db 238" "db 255" "db 255" "db 131" "db 196" "db 12" "db 131" "db 125" "db 20" "db 0" "db 116" "db 13" "db 255" "db 115" "db 16" "db 232" "db 197" "db 237" "db 255" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003ba80_2(void);
#pragma aux __0003ba80_2 = "db 131" "db 196" "db 4" "db 9" "db 198" "db 246" "db 67" "db 12" "db 8" "db 116" "db 18" "db 255" "db 115" "db 8" "db 232" "db 110" "db 240" "db 255" "db 255" "db 131" "db 196" "db 4" "db 199" "db 67" "db 8" "db 0" "db 0" "db 0" "db 0" "db 246" "db 67" "db 13" "db 8" "db 116" "db 29" "db 15" "db 182" "db 67" "db 25" "db 80" "db 141" "db 69" "db 240" "db 80" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003ba80_3(void);
#pragma aux __0003ba80_3 = "db 232" "db 248" "db 254" "db 255" "db 255" "db 131" "db 196" "db 8" "db 141" "db 69" "db 240" "db 80" "db 232" "db 84" "db 32" "db 0" "db 0" "db 131" "db 196" "db 4" "db 137" "db 240" "db 201" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux close_stream modify [eax ebx ecx edx esi edi ebp];
void close_stream(void)
{
    __0003ba80_0();
    __0003ba80_1();
    __0003ba80_2();
    __0003ba80_3();
}
