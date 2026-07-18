/* @ 0x39280 (44B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00039280_0(void);
#pragma aux __db_FUN_00039280_0 = "db 131" "db 251" "db 16" "db 115" "db 29" "db 209" "db 227" "db 209" "db 227" "db 139" "db 155" "db 250" "db 188" "db 0" "db 0" "db 131" "db 251" "db 0" "db 116" "db 14" "db 139" "db 11" "db 59" "db 200" "db 116" "db 14" "db 131" "db 195" "db 8" "db 131" "db 249" "db 255" "db 117" "db 242" "db 184" "db 0" "db 0" "db 0" "db 0" "db 195" "db 139" "db 67" "db 4" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux driver_msg_lookup modify [eax ebx ecx edx esi edi ebp];
void driver_msg_lookup(void) {
    __db_FUN_00039280_0();
}
