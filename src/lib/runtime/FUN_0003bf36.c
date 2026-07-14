/* C runtime: fixed-point / %f fraction formatter @ 0x3bf36 (CLIB3S, Watcom 9.5). Emits a
   signed value with a fractional field: '-' for negatives (negating [ebp-4]), converts the
   integer part via 0x3dbeb (_ltoa radix 10), appends '.' and precision ([esi+8]) fractional
   digits (repeated *10), then round-to-nearest carry propagation ('9'->'0' carry, bumping
   the preceding digit / leading '1'). Frame push ebx;esi;ebp; mov ebp,esp; sub esp,4; one
   literal rel32 call; trailing RET dropped. */
extern void __0003bf36_0(void);
#pragma aux __0003bf36_0 = "db 83" "db 86" "db 85" "db 137" "db 229" "db 131" "db 236" "db 4" "db 139" "db 93" "db 16" "db 139" "db 69" "db 20" "db 139" "db 117" "db 24" "db 137" "db 69" "db 252" "db 133" "db 192" "db 125" "db 7" "db 247" "db 93" "db 252" "db 198" "db 3" "db 45" "db 67" "db 131" "db 126" "db 8" "db 255" "db 117" "db 7" "db 199" "db 70" "db 8" "db 4" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bf36_1(void);
#pragma aux __0003bf36_1 = "db 106" "db 10" "db 83" "db 15" "db 183" "db 69" "db 254" "db 80" "db 232" "db 124" "db 28" "db 0" "db 0" "db 131" "db 196" "db 12" "db 137" "db 217" "db 141" "db 83" "db 1" "db 128" "db 59" "db 0" "db 116" "db 4" "db 137" "db 211" "db 235" "db 244" "db 131" "db 126" "db 8" "db 0" "db 116" "db 40" "db 198" "db 3" "db 46" "db 49" "db 192" "db 137" "db 211" "db 235" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bf36_2(void);
#pragma aux __0003bf36_2 = "db 23" "db 102" "db 199" "db 69" "db 254" "db 0" "db 0" "db 107" "db 85" "db 252" "db 10" "db 137" "db 85" "db 252" "db 138" "db 85" "db 254" "db 128" "db 194" "db 48" "db 64" "db 136" "db 19" "db 67" "db 59" "db 70" "db 8" "db 124" "db 228" "db 198" "db 3" "db 0" "db 246" "db 69" "db 253" "db 128" "db 116" "db 74" "db 57" "db 203" "db 117" "db 49" "db 141" "db 89" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bf36_3(void);
#pragma aux __0003bf36_3 = "db 1" "db 198" "db 1" "db 49" "db 141" "db 67" "db 1" "db 128" "db 59" "db 48" "db 117" "db 4" "db 137" "db 195" "db 235" "db 244" "db 128" "db 59" "db 46" "db 117" "db 17" "db 198" "db 3" "db 48" "db 141" "db 88" "db 1" "db 198" "db 0" "db 46" "db 128" "db 59" "db 48" "db 117" "db 3" "db 67" "db 235" "db 248" "db 198" "db 3" "db 48" "db 67" "db 198" "db 3" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003bf36_4(void);
#pragma aux __0003bf36_4 = "db 0" "db 235" "db 21" "db 75" "db 128" "db 59" "db 46" "db 117" "db 1" "db 75" "db 128" "db 59" "db 57" "db 116" "db 4" "db 254" "db 3" "db 235" "db 5" "db 198" "db 3" "db 48" "db 235" "db 182" "db 201" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003bf36 modify [eax ebx ecx edx esi edi ebp];
void FUN_0003bf36(void)
{
    __0003bf36_0();
    __0003bf36_1();
    __0003bf36_2();
    __0003bf36_3();
    __0003bf36_4();
}
