/* @ 0x3aea6 (50B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003aea6_0(void);
#pragma aux __db_FUN_0003aea6_0 = "db 83" "db 85" "db 137" "db 229" "db 139" "db 69" "db 12" "db 139" "db 85" "db 16" "db 139" "db 93" "db 20" "db 133" "db 219" "db 117" "db 4" "db 49" "db 192" "db 235" "db 26" "db 138" "db 8" "db 58" "db 10" "db 116" "db 10" "db 15" "db 182" "db 193" "db 15" "db 182" "db 18" "db 41" "db 208" "db 235" "db 10" "db 128" "db 56" "db 0" "db 116" "db 231" "db 75" "db 66" "db 64" "db 235" "db 222" "db 93" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003aea6_1(void);
#pragma aux __db_FUN_0003aea6_1 = "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003aea6 modify [eax ebx ecx edx esi edi ebp];
void FUN_0003aea6(void) {
    __db_FUN_0003aea6_0();
    __db_FUN_0003aea6_1();
}
