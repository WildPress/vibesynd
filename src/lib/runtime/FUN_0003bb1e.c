/* C runtime: _doprnt @ 0x3bb1e (CLIB3S, Watcom 9.5) -- the printf/sprintf formatter core
   (sprintf 0x3a4fa tails into it). Walks the format string, per '%' parses the spec via
   0x3bd37, fetches va_args and emits every output char through the caller put-function in
   EDI (call edi). Handles width, '0'/space pad, 'n' store-count (byte/word/dword via LES
   far ptr), 's'/'S' string args (byte/wide, near/far). Frame push ebx;esi;edi;es;ebp;
   mov ebp,esp; sub esp,0x4c. 2 direct calls (0x3bd37,0x3c019) literal rel32; RET dropped. */
extern void __0003bb1e_0(void);
#pragma aux __0003bb1e_0 = "db 83" "db 86" "db 87" "db 6" "db 85" "db 137" "db 229" "db 131" "db 236" "db 76" "db 139" "db 93" "db 32" "db 139" "db 125" "db 36" "db 139" "db 69" "db 24" "db 137" "db 69" "db 220" "db 49" "db 192" "db 136" "db 69" "db 240" "db 137" "db 69" "db 236" "db 139" "db 69" "db 28" "db 137" "db 69" "db 248" "db 235" "db 86" "db 141" "db 117" "db 220" "db 86" "db 83" "db 80" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_1(void);
#pragma aux __0003bb1e_1 = "db 232" "db 232" "db 1" "db 0" "db 0" "db 131" "db 196" "db 12" "db 137" "db 198" "db 138" "db 0" "db 70" "db 136" "db 69" "db 241" "db 137" "db 117" "db 248" "db 132" "db 192" "db 15" "db 132" "db 201" "db 1" "db 0" "db 0" "db 60" "db 110" "db 15" "db 133" "db 151" "db 0" "db 0" "db 0" "db 246" "db 69" "db 240" "db 32" "db 116" "db 78" "db 246" "db 69" "db 240" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_2(void);
#pragma aux __0003bb1e_2 = "db 128" "db 116" "db 16" "db 131" "db 3" "db 8" "db 139" "db 3" "db 196" "db 112" "db 248" "db 139" "db 69" "db 236" "db 38" "db 137" "db 6" "db 235" "db 17" "db 246" "db 69" "db 240" "db 64" "db 131" "db 3" "db 4" "db 139" "db 3" "db 139" "db 112" "db 252" "db 139" "db 69" "db 236" "db 137" "db 6" "db 139" "db 69" "db 248" "db 128" "db 56" "db 0" "db 15" "db 132" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_3(void);
#pragma aux __0003bb1e_3 = "db 136" "db 1" "db 0" "db 0" "db 139" "db 117" "db 248" "db 64" "db 128" "db 62" "db 37" "db 116" "db 149" "db 15" "db 182" "db 54" "db 86" "db 141" "db 117" "db 220" "db 86" "db 137" "db 69" "db 248" "db 255" "db 215" "db 131" "db 196" "db 8" "db 235" "db 217" "db 246" "db 69" "db 240" "db 16" "db 116" "db 45" "db 246" "db 69" "db 240" "db 128" "db 116" "db 18" "db 131" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_4(void);
#pragma aux __0003bb1e_4 = "db 3" "db 8" "db 139" "db 3" "db 196" "db 112" "db 248" "db 102" "db 139" "db 69" "db 236" "db 102" "db 38" "db 137" "db 6" "db 235" "db 187" "db 246" "db 69" "db 240" "db 64" "db 131" "db 3" "db 4" "db 139" "db 3" "db 139" "db 112" "db 252" "db 102" "db 139" "db 69" "db 236" "db 102" "db 137" "db 6" "db 235" "db 166" "db 246" "db 69" "db 240" "db 128" "db 15" "db 133" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_5(void);
#pragma aux __0003bb1e_5 = "db 123" "db 255" "db 255" "db 255" "db 246" "db 69" "db 240" "db 64" "db 235" "db 137" "db 141" "db 69" "db 220" "db 80" "db 83" "db 141" "db 69" "db 180" "db 80" "db 232" "db 7" "db 4" "db 0" "db 0" "db 142" "db 194" "db 131" "db 196" "db 12" "db 137" "db 198" "db 246" "db 69" "db 240" "db 8" "db 117" "db 31" "db 128" "db 125" "db 242" "db 32" "db 117" "db 25" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_6(void);
#pragma aux __0003bb1e_6 = "db 77" "db 224" "db 131" "db 125" "db 224" "db 0" "db 124" "db 16" "db 15" "db 182" "db 69" "db 242" "db 80" "db 141" "db 69" "db 220" "db 80" "db 255" "db 215" "db 131" "db 196" "db 8" "db 235" "db 231" "db 141" "db 69" "db 243" "db 137" "db 69" "db 252" "db 139" "db 69" "db 252" "db 128" "db 56" "db 0" "db 116" "db 18" "db 15" "db 182" "db 0" "db 80" "db 141" "db 69" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_7(void);
#pragma aux __0003bb1e_7 = "db 220" "db 80" "db 255" "db 69" "db 252" "db 255" "db 215" "db 131" "db 196" "db 8" "db 235" "db 230" "db 139" "db 69" "db 232" "db 255" "db 77" "db 232" "db 133" "db 192" "db 116" "db 13" "db 106" "db 48" "db 141" "db 69" "db 220" "db 80" "db 255" "db 215" "db 131" "db 196" "db 8" "db 235" "db 233" "db 246" "db 69" "db 240" "db 8" "db 117" "db 31" "db 128" "db 125" "db 242" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_8(void);
#pragma aux __0003bb1e_8 = "db 32" "db 116" "db 25" "db 255" "db 77" "db 224" "db 131" "db 125" "db 224" "db 0" "db 124" "db 16" "db 15" "db 182" "db 69" "db 242" "db 80" "db 141" "db 69" "db 220" "db 80" "db 255" "db 215" "db 131" "db 196" "db 8" "db 235" "db 231" "db 128" "db 125" "db 241" "db 115" "db 116" "db 6" "db 128" "db 125" "db 241" "db 83" "db 117" "db 62" "db 246" "db 69" "db 240" "db 32" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_9(void);
#pragma aux __0003bb1e_9 = "db 116" "db 29" "db 139" "db 69" "db 228" "db 255" "db 77" "db 228" "db 133" "db 192" "db 116" "db 73" "db 38" "db 15" "db 182" "db 6" "db 80" "db 141" "db 69" "db 220" "db 80" "db 131" "db 198" "db 2" "db 255" "db 215" "db 131" "db 196" "db 8" "db 235" "db 227" "db 139" "db 69" "db 228" "db 255" "db 77" "db 228" "db 133" "db 192" "db 116" "db 44" "db 38" "db 15" "db 182" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_10(void);
#pragma aux __0003bb1e_10 = "db 6" "db 80" "db 141" "db 69" "db 220" "db 80" "db 70" "db 255" "db 215" "db 131" "db 196" "db 8" "db 235" "db 229" "db 139" "db 69" "db 228" "db 255" "db 77" "db 228" "db 133" "db 192" "db 116" "db 17" "db 38" "db 15" "db 182" "db 6" "db 80" "db 141" "db 69" "db 220" "db 80" "db 70" "db 255" "db 215" "db 131" "db 196" "db 8" "db 235" "db 229" "db 246" "db 69" "db 240" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_11(void);
#pragma aux __0003bb1e_11 = "db 8" "db 15" "db 132" "db 145" "db 254" "db 255" "db 255" "db 131" "db 125" "db 224" "db 0" "db 15" "db 142" "db 135" "db 254" "db 255" "db 255" "db 139" "db 69" "db 224" "db 255" "db 77" "db 224" "db 133" "db 192" "db 15" "db 132" "db 121" "db 254" "db 255" "db 255" "db 106" "db 32" "db 141" "db 69" "db 220" "db 80" "db 255" "db 215" "db 131" "db 196" "db 8" "db 235" "db 229" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bb1e_12(void);
#pragma aux __0003bb1e_12 = "db 139" "db 69" "db 236" "db 201" "db 7" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003bb1e modify [eax ebx ecx edx esi edi ebp];
void FUN_0003bb1e(void)
{
    __0003bb1e_0();
    __0003bb1e_1();
    __0003bb1e_2();
    __0003bb1e_3();
    __0003bb1e_4();
    __0003bb1e_5();
    __0003bb1e_6();
    __0003bb1e_7();
    __0003bb1e_8();
    __0003bb1e_9();
    __0003bb1e_10();
    __0003bb1e_11();
    __0003bb1e_12();
}
