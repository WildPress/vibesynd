/* @ 0x39966 (46B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00039966_0(void);
#pragma aux __db_FUN_00039966_0 = "db 85" "db 139" "db 236" "db 86" "db 87" "db 156" "db 250" "db 139" "db 93" "db 8" "db 131" "db 251" "db 16" "db 115" "db 14" "db 209" "db 227" "db 209" "db 227" "db 199" "db 131" "db 250" "db 188" "db 0" "db 0" "db 0" "db 0" "db 0" "db 0" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux unregister_driver modify [eax ebx ecx edx esi edi ebp];
void unregister_driver(void) {
    __db_FUN_00039966_0();
}
