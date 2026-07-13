/* C runtime: printf flag-char parser @ 0x3be40 (CLIB3S, Watcom 9.5). Scans the leading
   conversion flags at [ebp+8] into flags byte [rec+0x14] (rec=[ebp+0xc]): '-'->0x8,
   '#'->0x1, '+'->0x4 & clear 0x2, ' '->0x2 (unless '+' seen), '0'->pad char [rec+0x16]=0x30.
   Stops at first non-flag, returns ptr to it. Plain ebp-frame leaf; full-body db, RET dropped. */
extern void __0003be40_0(void);
#pragma aux __0003be40_0 = "db 85" "db 137" "db 229" "db 139" "db 85" "db 8" "db 139" "db 69" "db 12" "db 198" "db 64" "db 20" "db 0" "db 128" "db 58" "db 45" "db 117" "db 6" "db 128" "db 72" "db 20" "db 8" "db 235" "db 52" "db 128" "db 58" "db 35" "db 117" "db 6" "db 128" "db 72" "db 20" "db 1" "db 235" "db 41" "db 128" "db 58" "db 43" "db 117" "db 10" "db 128" "db 72" "db 20" "db 4" modify exact [eax ebx ecx edx esi edi ebp];
extern void __0003be40_1(void);
#pragma aux __0003be40_1 = "db 128" "db 96" "db 20" "db 253" "db 235" "db 26" "db 128" "db 58" "db 32" "db 117" "db 12" "db 246" "db 64" "db 20" "db 4" "db 117" "db 15" "db 128" "db 72" "db 20" "db 2" "db 235" "db 9" "db 128" "db 58" "db 48" "db 117" "db 7" "db 198" "db 64" "db 22" "db 48" "db 66" "db 235" "db 190" "db 137" "db 208" "db 93" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003be40 modify [eax ebx ecx edx esi edi ebp];
void FUN_0003be40(void)
{
    __0003be40_0();
    __0003be40_1();
}
