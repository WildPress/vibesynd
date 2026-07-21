/* @ 0x29988 (60B) -- db-transcription (separate predicate function the manifest's oversized
   update_static_object_states @0x29088 [size 2368] overran). short f(uchar*p): advances p[0xc]
   through the g_pool table @0x5338 (8-byte stride, [+6] next-index), returns [+5]&1. No calls,
   one baked data ref [0x5338] (fixed DGROUP addr, as every db-transcription). Carved so the
   mission interpreters' FUN_00029988(p) references resolve to the real code, not a ret-stub. */
extern void __db_FUN_00029988_0(void);
#pragma aux __db_FUN_00029988_0 = "db 83" "db 139" "db 84" "db 36" "db 8" "db 49" "db 192" "db 102" "db 139" "db 66" "db 12" "db 139" "db 29" "db 56" "db 83" "db 0" "db 0" "db 141" "db 4" "db 197" "db 0" "db 0" "db 0" "db 0" "db 1" "db 216" "db 102" "db 139" "db 64" "db 6" modify exact [eax ebx ecx edx esi edi];
extern void __db_FUN_00029988_1(void);
#pragma aux __db_FUN_00029988_1 = "db 102" "db 137" "db 66" "db 12" "db 49" "db 192" "db 102" "db 139" "db 66" "db 12" "db 141" "db 4" "db 197" "db 0" "db 0" "db 0" "db 0" "db 1" "db 216" "db 246" "db 64" "db 5" "db 1" "db 15" "db 149" "db 192" "db 48" "db 228" "db 91" "db 195" modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_00029988 modify [eax ebx ecx edx esi edi];
void FUN_00029988(void) { __db_FUN_00029988_0(); __db_FUN_00029988_1(); }
