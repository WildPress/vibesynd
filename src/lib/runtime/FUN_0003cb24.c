/* C runtime: stream mode/reopen helper @ 0x3cb24 (CLIB3S, Watcom 9.5). Framed
   (push ebx/esi/edi/ebp; mov ebp,esp; sub esp,4). Takes a FILE* at [ebp+0x14]; saves
   and clears its buffering-mode bits (flags byte at +0xc, mask 0x30 -> [ebp-4], then
   &=0xcf), lazily allocates the file's device via ioalloc when the fd slot (+8)
   is 0, toggles the +0xd binary/text flag, then calls the core FUN_0003bb1e with a
   literal format ptr (0x2f3c2) plus the two extra args, optionally re-syncs via
   FUN_0003d966, folds a -1 error when the 0x20 flag is set, and restores the saved
   mode bits. Whole body db-transcribed into a frameless wrapper; three CALL rel32s are
   masked externs, 0x2f3c2 is a literal db abs32; trailing RET supplied by wrapper. */
extern void ioalloc(void);
extern void FUN_0003bb1e(void);
extern void FUN_0003d966(void);
extern void __FUN_0003cb24_0(void);
extern void __FUN_0003cb24_1(void);
#pragma aux __FUN_0003cb24_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 131" "db 236" "db 4" "db 139" "db 93" "db 20" "db 139" "db 115" "db 12" "db 131" "db 230" "db 48" "db 128" "db 99" "db 12" "db 207" "db 137" "db 117" "db 252" "db 131" "db 123" "db 8" "db 0" "db 117" "db 9" "db 83" "call ioalloc" "db 131" "db 196" "db 4" "db 49" "db 255" "db 246" "db 67" "db 13" "db 4" "db 116" "db 13" "db 128" "db 99" "db 13" "db 251" "db 191" "db 1" "db 0" "db 0" "db 0" "db 128" "db 75" "db 13" "db 1" "db 104" "db 194" "db 243" "db 2" "db 0" "db 255" "db 117" "db 28" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cb24_1 = "db 117" "db 24" "db 83" "call FUN_0003bb1e" "db 137" "db 198" "db 131" "db 196" "db 16" "db 133" "db 255" "db 116" "db 17" "db 128" "db 99" "db 13" "db 254" "db 83" "db 128" "db 75" "db 13" "db 4" "call FUN_0003d966" "db 131" "db 196" "db 4" "db 246" "db 67" "db 12" "db 32" "db 116" "db 5" "db 190" "db 255" "db 255" "db 255" "db 255" "db 139" "db 125" "db 252" "db 137" "db 240" "db 9" "db 123" "db 12" "db 201" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003cb24 modify [eax ebx ecx edx esi edi ebp];
void FUN_0003cb24(void)
{
    __FUN_0003cb24_0();
    __FUN_0003cb24_1();
}
