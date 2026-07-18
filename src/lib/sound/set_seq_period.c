/* @ 0x397f1 (85B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_000397f1_0(void);
#pragma aux __db_FUN_000397f1_0 = "db 85" "db 139" "db 236" "db 86" "db 87" "db 156" "db 250" "db 139" "db 93" "db 8" "db 209" "db 227" "db 15" "db 183" "db 131" "db 56" "db 188" "db 0" "db 0" "db 80" "db 102" "db 199" "db 131" "db 56" "db 188" "db 0" "db 0" "db 1" "db 0" "db 209" "db 227" "db 139" "db 69" "db 12" "db 137" "db 131" "db 158" "db 188" "db 0" "db 0" "db 199" "db 131" "db 90" "db 188" "db 0" "db 0" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_000397f1_1(void);
#pragma aux __db_FUN_000397f1_1 = "db 0" "db 0" "db 232" "db 158" "db 252" "db 255" "db 255" "db 88" "db 139" "db 93" "db 8" "db 209" "db 227" "db 102" "db 137" "db 131" "db 56" "db 188" "db 0" "db 0" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux set_seq_period modify [eax ebx ecx edx esi edi ebp];
void set_seq_period(void) {
    __db_FUN_000397f1_0();
    __db_FUN_000397f1_1();
}
