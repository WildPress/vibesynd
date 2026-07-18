/* @ 0x4987e (113B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004987e_0(void);
#pragma aux __db_FUN_0004987e_0 = "db 85" "db 139" "db 236" "db 80" "db 83" "db 81" "db 82" "db 87" "db 86" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 2" "db 117" "db 9" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 4" "db 116" "db 50" "db 102" "db 187" "db 0" "db 0" "db 102" "db 185" "db 16" "db 0" "db 102" "db 83" "db 102" "db 81" "db 102" "db 184" "db 0" "db 16" "db 205" "db 16" "db 102" "db 89" "db 102" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004987e_1(void);
#pragma aux __db_FUN_0004987e_1 = "db 91" "db 254" "db 199" "db 254" "db 195" "db 103" "db 226" "db 235" "db 139" "db 117" "db 8" "db 102" "db 186" "db 200" "db 3" "db 176" "db 0" "db 238" "db 178" "db 201" "db 185" "db 48" "db 0" "db 0" "db 0" "db 243" "db 110" "db 235" "db 28" "db 246" "db 5" "db 5" "db 1" "db 0" "db 0" "db 1" "db 116" "db 19" "db 139" "db 117" "db 8" "db 102" "db 186" "db 200" "db 3" "db 176" "db 0" "db 238" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0004987e_2(void);
#pragma aux __db_FUN_0004987e_2 = "db 178" "db 201" "db 185" "db 0" "db 3" "db 0" "db 0" "db 243" "db 110" "db 94" "db 95" "db 90" "db 89" "db 91" "db 88" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux upload_palette modify [eax ebx ecx edx esi edi ebp];
void upload_palette(void) { __db_FUN_0004987e_0(); __db_FUN_0004987e_1(); __db_FUN_0004987e_2(); }
