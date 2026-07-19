/* C runtime: printf conversion-spec parser @ 0x3bd37 (CLIB3S, Watcom 9.5). Called by
   _doprnt per '%': parses flag chars via 0x3be40, then field width (digits or '*'=va_arg),
   precision ('.' then digits or '*'), and length/type modifier (l/L 'w'->0x20,'h'->0x10,
   'F'->0x80,'N'->0x40) into the spec record [ebp+0x18] (+4 width,+8 prec,+0x14 flags).
   Returns ptr past the spec. Regs-first frame push ebx;esi;ebp; mov ebp,esp; one literal
   rel32 call; trailing RET dropped. */
extern void __0003bd37_0(void);
#pragma aux __0003bd37_0 = "db 83" "db 86" "db 85" "db 137" "db 229" "db 139" "db 69" "db 16" "db 139" "db 117" "db 20" "db 139" "db 93" "db 24" "db 83" "db 198" "db 67" "db 23" "db 0" "db 80" "db 198" "db 67" "db 22" "db 32" "db 232" "db 236" "db 0" "db 0" "db 0" "db 137" "db 194" "db 199" "db 67" "db 4" "db 0" "db 0" "db 0" "db 0" "db 131" "db 196" "db 8" "db 128" "db 56" "db 48" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bd37_1(void);
#pragma aux __0003bd37_1 = "db 114" "db 33" "db 128" "db 56" "db 57" "db 119" "db 28" "db 107" "db 75" "db 4" "db 10" "db 15" "db 182" "db 16" "db 131" "db 234" "db 48" "db 1" "db 209" "db 64" "db 137" "db 75" "db 4" "db 128" "db 56" "db 48" "db 114" "db 35" "db 128" "db 56" "db 57" "db 118" "db 230" "db 235" "db 28" "db 128" "db 56" "db 42" "db 117" "db 23" "db 131" "db 6" "db 4" "db 139" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bd37_2(void);
#pragma aux __0003bd37_2 = "db 22" "db 139" "db 82" "db 252" "db 137" "db 83" "db 4" "db 133" "db 210" "db 125" "db 7" "db 247" "db 91" "db 4" "db 128" "db 75" "db 20" "db 8" "db 64" "db 199" "db 67" "db 8" "db 255" "db 255" "db 255" "db 255" "db 128" "db 56" "db 46" "db 117" "db 66" "db 64" "db 199" "db 67" "db 8" "db 0" "db 0" "db 0" "db 0" "db 128" "db 56" "db 42" "db 117" "db 25" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bd37_3(void);
#pragma aux __0003bd37_3 = "db 131" "db 6" "db 4" "db 139" "db 22" "db 139" "db 82" "db 252" "db 137" "db 83" "db 8" "db 133" "db 210" "db 125" "db 7" "db 199" "db 67" "db 8" "db 255" "db 255" "db 255" "db 255" "db 64" "db 235" "db 28" "db 128" "db 56" "db 48" "db 114" "db 23" "db 128" "db 56" "db 57" "db 119" "db 18" "db 107" "db 75" "db 8" "db 10" "db 15" "db 182" "db 16" "db 131" "db 234" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bd37_4(void);
#pragma aux __0003bd37_4 = "db 48" "db 1" "db 209" "db 64" "db 137" "db 75" "db 8" "db 235" "db 228" "db 138" "db 8" "db 141" "db 80" "db 1" "db 128" "db 249" "db 78" "db 114" "db 23" "db 118" "db 58" "db 128" "db 249" "db 108" "db 114" "db 9" "db 118" "db 28" "db 128" "db 249" "db 119" "db 116" "db 23" "db 235" "db 50" "db 128" "db 249" "db 104" "db 116" "db 23" "db 235" "db 43" "db 128" "db 249" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bd37_5(void);
#pragma aux __0003bd37_5 = "db 70" "db 114" "db 38" "db 118" "db 22" "db 128" "db 249" "db 76" "db 116" "db 25" "db 235" "db 29" "db 64" "db 128" "db 75" "db 20" "db 32" "db 235" "db 22" "db 137" "db 208" "db 128" "db 75" "db 20" "db 16" "db 235" "db 14" "db 137" "db 208" "db 128" "db 75" "db 20" "db 128" "db 235" "db 6" "db 137" "db 208" "db 128" "db 75" "db 20" "db 64" "db 93" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux parse_conv_spec modify [eax ebx ecx edx esi edi ebp];
void parse_conv_spec(void)
{
    __0003bd37_0();
    __0003bd37_1();
    __0003bd37_2();
    __0003bd37_3();
    __0003bd37_4();
    __0003bd37_5();
}
