/* @ 0x4a69c (44B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0004a69c_0(void);
#pragma aux __db_FUN_0004a69c_0 = "db 85" "db 139" "db 236" "db 83" "db 81" "db 82" "db 86" "db 87" "db 102" "db 139" "db 69" "db 8" "db 102" "db 139" "db 93" "db 12" "db 138" "db 77" "db 16" "db 138" "db 109" "db 20" "db 102" "db 186" "db 0" "db 0" "db 139" "db 125" "db 28" "db 139" "db 117" "db 32" "db 232" "db 178" "db 9" "db 0" "db 0" "db 95" "db 94" "db 90" "db 89" "db 91" "db 201" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux blit_sprite modify [eax ebx ecx edx esi edi ebp];
void blit_sprite(void) { __db_FUN_0004a69c_0(); }
