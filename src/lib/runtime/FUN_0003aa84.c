/* C runtime: malloc / _nmalloc core @ 0x3aa84 (CLIB3S, small model). Saves ES/FS/GS, reads DS
   as the near selector, walks/extends the near heap via 0x3c594 (alloc-from-freelist), growing
   with 0x3c816 then 0x3ca0d (brk/_nheapgrow) and retrying; clears the "no-retry" byte at 0x11e48
   and returns the block (EBX) or 0. Body db-transcribed incl. the push es/fs/gs segment saves,
   split into inline #pragma-aux parts; Watcom adds only ret. */
extern void *FUN_0003c594(void);
extern int FUN_0003c816(unsigned);
extern int FUN_0003ca0d(unsigned);
extern void __mal95_1(void);
#pragma aux __mal95_1 = "db 83" "db 86" "db 6" "db 15" "db 160" "db 15" "db 168" "db 85" "db 137" "db 229" "db 131" "db 236" "db 4" "db 139" "db 117" "db 28" "db 133" "db 246" "db 117" "db 4" "db 49" "db 192" "db 235" "db 81" "db 198" "db 69" "db 252" "db 0" "db 184" "db 196" "db 191" "db 0" "db 0" parm [] modify exact [eax ebx ecx edx esi];
extern void __mal95_2(void);
#pragma aux __mal95_2 = "db 140" "db 218" "db 187" "db 204" "db 191" "db 0" "db 0" "db 15" "db 183" "db 210" "db 137" "db 240" "call FUN_0003c594" "db 137" "db 195" "db 133" "db 192" "db 117" "db 40" "db 128" "db 125" "db 252" "db 0" "db 117" "db 13" "db 86" "call FUN_0003c816" "db 131" "db 196" "db 4" "db 133" "db 192" "db 117" parm [] modify exact [eax ebx ecx edx esi];
extern void __mal95_3(void);
#pragma aux __mal95_3 = "db 15" "db 86" "call FUN_0003ca0d" "db 131" "db 196" "db 4" "db 133" "db 192" "db 116" "db 8" "db 235" "db 190" "db 198" "db 69" "db 252" "db 1" "db 235" "db 188" "db 198" "db 5" "db 72" "db 30" "db 1" "db 0" "db 0" "db 137" "db 216" "db 201" "db 15" "db 169" "db 15" "db 161" "db 7" parm [] modify exact [eax ebx ecx edx esi];
extern long __mal95_4(void);
#pragma aux __mal95_4 = "db 94" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi];
#pragma aux FUN_0003aa84 modify [eax ebx ecx edx esi];
void *FUN_0003aa84(unsigned size)
{
    __mal95_1();
    __mal95_2();
    __mal95_3();
    return __mal95_4();
}
