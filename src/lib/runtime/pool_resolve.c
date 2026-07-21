/* @ 0x37608 (80B) -- db-transcription (hand-asm/library). */

extern void __db_pool_resolve_0(void);
#pragma aux __db_pool_resolve_0 = "db 83" "db 86" "db 139" "db 116" "db 36" "db 16" "db 139" "db 84" "db 36" "db 12" "db 49" "db 192" "db 102" "db 139" "db 66" "db 58" "db 185" "db 1" "db 0" "db 0" "db 0" "db 5" "db 14" "db 129" "db 0" "db 0" "db 49" "db 219" "db 15" "db 191" "db 209" "db 102" "db 137" "db 243" "db 57" "db 218" "db 125" "db 25" "db 102" "db 139" "db 80" "db 28" "db 102" "db 133" "db 210" "db 116" "db 13" "db 102" modify exact [eax ebx ecx edx esi edi ebp];
extern void __db_pool_resolve_1(void);
#pragma aux __db_pool_resolve_1 = "db 137" "db 208" "db 37" "db 255" "db 255" "db 0" "db 0" "db 5" "db 14" "db 129" "db 0" "db 0" "db 65" "db 235" "db 219" "db 94" "db 91" "db 195" "db 141" "db 128" "db 0" "db 0" "db 0" "db 0" "db 141" "db 146" "db 0" "db 0" "db 0" "db 0" "db 139" "db 192" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux pool_resolve modify [eax ebx ecx edx esi edi ebp] aborts;
void pool_resolve(void) {
    __db_pool_resolve_0();
    __db_pool_resolve_1();
}
