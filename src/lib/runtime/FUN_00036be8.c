/* @ 0x36be8 (58B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00036be8_0(void);
#pragma aux __db_FUN_00036be8_0 = "db 139" "db 84" "db 36" "db 4" "db 102" "db 131" "db 122" "db 36" "db 0" "db 116" "db 44" "db 139" "db 68" "db 36" "db 8" "db 102" "db 139" "db 64" "db 28" "db 102" "db 133" "db 192" "db 116" "db 31" "db 102" "db 133" "db 192" "db 116" "db 26" "db 37" "db 255" "db 255" "db 0" "db 0" "db 5" "db 14" "db 129" "db 0" "db 0" "db 57" "db 208" "db 117" "db 6" "db 184" "db 1" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00036be8_1(void);
#pragma aux __db_FUN_00036be8_1 = "db 195" "db 102" "db 139" "db 64" "db 34" "db 235" "db 225" "db 49" "db 192" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00036be8 modify [eax ebx ecx edx esi edi ebp];
void FUN_00036be8(void) {
    __db_FUN_00036be8_0();
    __db_FUN_00036be8_1();
}
