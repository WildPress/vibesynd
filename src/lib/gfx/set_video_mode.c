/* @ 0x498ef (51B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_000498ef_0(void);
#pragma aux __db_FUN_000498ef_0 = "db 85" "db 139" "db 236" "db 80" "db 83" "db 81" "db 82" "db 87" "db 86" "db 102" "db 139" "db 69" "db 8" "db 180" "db 0" "db 205" "db 16" "db 102" "db 184" "db 7" "db 0" "db 102" "db 51" "db 201" "db 102" "db 186" "db 126" "db 2" "db 205" "db 51" "db 102" "db 184" "db 8" "db 0" "db 102" "db 51" "db 201" "db 102" "db 186" "db 142" "db 1" "db 205" "db 51" "db 94" "db 95" "db 90" "db 89" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_000498ef_1(void);
#pragma aux __db_FUN_000498ef_1 = "db 88" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux set_video_mode modify [eax ebx ecx edx esi edi ebp];
void set_video_mode(void) { __db_FUN_000498ef_0(); __db_FUN_000498ef_1(); }
