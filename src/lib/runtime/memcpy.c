/* @ 0x3d90b (42B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003d90b_0(void);
#pragma aux __db_FUN_0003d90b_0 = "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 77" "db 24" "db 139" "db 125" "db 16" "db 139" "db 117" "db 20" "db 6" "db 140" "db 216" "db 142" "db 192" "db 87" "db 137" "db 200" "db 193" "db 233" "db 2" "db 242" "db 165" "db 138" "db 200" "db 128" "db 225" "db 3" "db 242" "db 164" "db 95" "db 7" "db 137" "db 248" "db 93" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux memcpy modify [eax ebx ecx edx esi edi ebp];
void memcpy(void) { __db_FUN_0003d90b_0(); }
