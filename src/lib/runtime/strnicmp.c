/* @ 0x3deee (77B) -- db-transcription (hand-asm/library). */

extern void __db_FUN_0003deee_0(void);
#pragma aux __db_FUN_0003deee_0 = "db 83" "db 85" "db 137" "db 229" "db 139" "db 85" "db 12" "db 139" "db 69" "db 16" "db 139" "db 93" "db 20" "db 133" "db 219" "db 117" "db 4" "db 49" "db 192" "db 235" "db 53" "db 138" "db 10" "db 138" "db 40" "db 128" "db 249" "db 65" "db 114" "db 8" "db 128" "db 249" "db 90" "db 119" "db 3" "db 128" "db 193" "db 32" "db 128" "db 253" "db 65" "db 114" "db 8" "db 128" "db 253" "db 90" "db 119" "db 3" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_FUN_0003deee_1(void);
#pragma aux __db_FUN_0003deee_1 = "db 128" "db 197" "db 32" "db 56" "db 233" "db 116" "db 10" "db 15" "db 182" "db 193" "db 15" "db 182" "db 213" "db 41" "db 208" "db 235" "db 9" "db 132" "db 237" "db 116" "db 204" "db 75" "db 64" "db 66" "db 235" "db 195" "db 93" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux strnicmp modify [eax ebx ecx edx esi edi ebp];
void strnicmp(void) { __db_FUN_0003deee_0(); __db_FUN_0003deee_1(); }
