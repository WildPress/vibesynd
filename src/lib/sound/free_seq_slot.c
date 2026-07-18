/* @ 0x396d5 (77B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_000396d5_0(void);
#pragma aux __db_FUN_000396d5_0 = "db 85" "db 139" "db 236" "db 86" "db 87" "db 156" "db 250" "db 139" "db 93" "db 8" "db 131" "db 251" "db 255" "db 116" "db 45" "db 209" "db 227" "db 102" "db 131" "db 187" "db 56" "db 188" "db 0" "db 0" "db 0" "db 116" "db 33" "db 102" "db 199" "db 131" "db 56" "db 188" "db 0" "db 0" "db 0" "db 0" "db 102" "db 255" "db 13" "db 240" "db 187" "db 0" "db 0" "db 117" "db 15" "db 106" "db 0" "db 232" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_000396d5_1(void);
#pragma aux __db_FUN_000396d5_1 = "db 94" "db 253" "db 255" "db 255" "db 131" "db 196" "db 4" "db 232" "db 30" "db 253" "db 255" "db 255" "db 85" "db 139" "db 236" "db 246" "db 69" "db 5" "db 2" "db 250" "db 116" "db 1" "db 251" "db 93" "db 157" "db 95" "db 94" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux free_seq_slot modify [eax ebx ecx edx esi edi ebp];
void free_seq_slot(void) {
    __db_FUN_000396d5_0();
    __db_FUN_000396d5_1();
}
