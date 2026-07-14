/* @ 0x39879 (69B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_00039879_0(void);
#pragma aux __db_FUN_00039879_0 = "db 85" "db 139" "db 236" "db 86" "db 87" "db 156" "db 250" "db 131" "db 125" "db 12" "db 0" "db 117" "db 7" "db 184" "db 141" "db 214" "db 0" "db 0" "db 235" "db 20" "db 184" "db 16" "db 39" "db 0" "db 0" "db 186" "db 0" "db 0" "db 0" "db 0" "db 187" "db 156" "db 46" "db 0" "db 0" "db 247" "db 101" "db 12" "db 247" "db 243" "db 80" "db 255" "db 117" "db 8" "db 232" "db 71" "db 255" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_00039879_1(void);
#pragma aux __db_FUN_00039879_1 = "db 255" "db 131" "db 196" "db 8" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_00039879 modify [eax ebx ecx edx esi edi ebp];
void FUN_00039879(void) {
    __db_FUN_00039879_0();
    __db_FUN_00039879_1();
}
