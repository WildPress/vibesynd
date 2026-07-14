/* C runtime: integer-to-padded-field helper @ 0x3bee3 (CLIB3S, Watcom 9.5). Converts a
   value to text via 0x3dbeb (_ltoa, radix 0x10 pushed) into a scratch buffer, gets its
   length via 0x3dc1b (strlen), then right-justifies the digits into a field of width
   [ebp+0x1c] and zero-fills the leading gap, NUL-terminating. Frame push ebx;esi;edi;ebp;
   mov ebp,esp; two literal rel32 calls; trailing RET dropped. */
extern void __0003bee3_0(void);
#pragma aux __0003bee3_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 125" "db 24" "db 106" "db 16" "db 87" "db 255" "db 117" "db 20" "db 232" "db 244" "db 28" "db 0" "db 0" "db 131" "db 196" "db 12" "db 87" "db 137" "db 254" "db 232" "db 25" "db 29" "db 0" "db 0" "db 137" "db 194" "db 139" "db 69" "db 28" "db 131" "db 196" "db 4" "db 72" "db 1" "db 250" "db 141" "db 28" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bee3_1(void);
#pragma aux __0003bee3_1 = "db 56" "db 235" "db 7" "db 74" "db 138" "db 10" "db 72" "db 136" "db 11" "db 75" "db 57" "db 242" "db 117" "db 245" "db 141" "db 20" "db 56" "db 133" "db 192" "db 124" "db 7" "db 72" "db 198" "db 2" "db 48" "db 74" "db 235" "db 245" "db 3" "db 125" "db 28" "db 198" "db 7" "db 0" "db 93" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003bee3 modify [eax ebx ecx edx esi edi ebp];
void FUN_0003bee3(void)
{
    __0003bee3_0();
    __0003bee3_1();
}
