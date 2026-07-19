/* @ 0x1bb48 (224B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0001bb48_0(void);
#pragma aux __db_FUN_0001bb48_0 = "db 139" "db 84" "db 36" "db 4" "db 139" "db 68" "db 36" "db 8" "db 128" "db 61" "db 204" "db 226" "db 0" "db 0" "db 0" "db 116" "db 2" "db 49" "db 192" "db 128" "db 61" "db 207" "db 226" "db 0" "db 0" "db 0" "db 116" "db 2" "db 49" "db 210" "db 128" "db 61" "db 212" "db 226" "db 0" "db 0" "db 0" "db 116" "db 5" "db 184" "db 142" "db 1" "db 0" "db 0" "db 128" "db 61" "db 209" "db 226" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001bb48_1(void);
#pragma aux __db_FUN_0001bb48_1 = "db 0" "db 0" "db 0" "db 116" "db 7" "db 186" "db 126" "db 2" "db 0" "db 0" "db 235" "db 28" "db 102" "db 133" "db 210" "db 117" "db 23" "db 102" "db 133" "db 192" "db 117" "db 4" "db 106" "db 5" "db 235" "db 91" "db 102" "db 61" "db 142" "db 1" "db 117" "db 4" "db 106" "db 9" "db 235" "db 81" "db 106" "db 1" "db 235" "db 77" "db 102" "db 133" "db 192" "db 117" "db 29" "db 102" "db 129" "db 250" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001bb48_2(void);
#pragma aux __db_FUN_0001bb48_2 = "db 126" "db 2" "db 117" "db 11" "db 106" "db 6" "db 232" "db 117" "db 0" "db 0" "db 0" "db 131" "db 196" "db 4" "db 195" "db 106" "db 4" "db 232" "db 106" "db 0" "db 0" "db 0" "db 131" "db 196" "db 4" "db 195" "db 102" "db 129" "db 250" "db 126" "db 2" "db 117" "db 28" "db 102" "db 61" "db 142" "db 1" "db 117" "db 11" "db 106" "db 10" "db 232" "db 82" "db 0" "db 0" "db 0" "db 131" "db 196" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001bb48_3(void);
#pragma aux __db_FUN_0001bb48_3 = "db 4" "db 195" "db 106" "db 2" "db 232" "db 71" "db 0" "db 0" "db 0" "db 131" "db 196" "db 4" "db 195" "db 102" "db 61" "db 142" "db 1" "db 117" "db 10" "db 106" "db 8" "db 232" "db 54" "db 0" "db 0" "db 0" "db 131" "db 196" "db 4" "db 195" "db 139" "db 192" "db 174" "db 230" "db 0" "db 0" "db 64" "db 230" "db 0" "db 0" "db 12" "db 234" "db 0" "db 0" "db 187" "db 229" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0001bb48_4(void);
#pragma aux __db_FUN_0001bb48_4 = "db 32" "db 231" "db 0" "db 0" "db 212" "db 231" "db 0" "db 0" "db 12" "db 234" "db 0" "db 0" "db 34" "db 229" "db 0" "db 0" "db 113" "db 232" "db 0" "db 0" "db 59" "db 233" "db 0" "db 0" "db 141" "db 128" "db 0" "db 0" "db 0" "db 0" "db 139" "db 210" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux edge_scroll_dispatch modify [eax ebx ecx edx esi edi ebp] aborts;
void edge_scroll_dispatch(void) {
    __db_FUN_0001bb48_0();
    __db_FUN_0001bb48_1();
    __db_FUN_0001bb48_2();
    __db_FUN_0001bb48_3();
    __db_FUN_0001bb48_4();
}
