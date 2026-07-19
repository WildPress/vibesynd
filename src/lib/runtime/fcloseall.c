/* @ 0x3dea0 (78B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003dea0_0(void);
#pragma aux __db_FUN_0003dea0_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 107" "db 125" "db 20" "db 26" "db 161" "db 76" "db 30" "db 1" "db 0" "db 49" "db 219" "db 129" "db 199" "db 128" "db 192" "db 0" "db 0" "db 235" "db 42" "db 139" "db 48" "db 139" "db 64" "db 4" "db 186" "db 1" "db 0" "db 0" "db 0" "db 246" "db 64" "db 13" "db 64" "db 117" "db 13" "db 57" "db 248" "db 114" "db 20" "db 61" "db 2" "db 193" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003dea0_1(void);
#pragma aux __db_FUN_0003dea0_1 = "db 0" "db 0" "db 115" "db 2" "db 49" "db 210" "db 82" "db 80" "db 67" "db 232" "db 236" "db 218" "db 255" "db 255" "db 131" "db 196" "db 8" "db 137" "db 240" "db 133" "db 192" "db 117" "db 210" "db 137" "db 216" "db 93" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux fcloseall modify [eax ebx ecx edx esi edi ebp];
void fcloseall(void) { __db_FUN_0003dea0_0(); __db_FUN_0003dea0_1(); }
