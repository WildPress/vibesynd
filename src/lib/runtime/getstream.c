/* C runtime: _getstream/freelist search @ 0x3b90d (CLIB3S). Walks the two FILE freelists (g_11e4c/g_11e44); regs-first prologue -> full db-transcription.
   Body db-transcribed (frameless wrapper; modify[] suppresses the wrapper frame).
   External calls are real masked relocs; abs data refs are literal bytes. */
extern void close_stream();
extern void errno_ptr();
extern void __db_0003b90d_0(void);
#pragma aux __db_0003b90d_0 = "db 83" "db 85" "db 137" "db 229" "db 139" "db 93" "db 12" "db 161" "db 76" "db 30" "db 1" "db 0" "db 133" "db 192" "db 116" "db 30" "db 59" "db 88" "db 4" "db 117" "db 21" "db 246" "db 67" "db 12" "db 3" "db 116" "db 11" "db 106" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b90d_1(void);
#pragma aux __db_0003b90d_1 = "db 1" "db 83" "call close_stream" "db 131" "db 196" "db 8" "db 137" "db 216" "db 235" "db 56" "db 139" "db 0" "db 235" "db 222" "db 186" "db 68" "db 30" "db 1" "db 0" "db 139" "db 2" "db 133" "db 192" "db 116" "db 28" "db 59" "db 88" "db 4" parm [] modify exact [eax ebx ecx edx esi edi];
extern void __db_0003b90d_2(void);
#pragma aux __db_0003b90d_2 = "db 117" "db 19" "db 139" "db 8" "db 137" "db 10" "db 139" "db 21" "db 76" "db 30" "db 1" "db 0" "db 137" "db 16" "db 163" "db 76" "db 30" "db 1" "db 0" "db 235" "db 213" "db 137" "db 194" "db 235" "db 222" "call errno_ptr" "db 199" "db 0" parm [] modify exact [eax ebx ecx edx esi edi];
extern void * __db_0003b90d_3(void);
#pragma aux __db_0003b90d_3 = "db 4" "db 0" "db 0" "db 0" "db 49" "db 192" "db 93" "db 91" parm [] value [eax] modify exact [eax ebx ecx edx esi edi];
#pragma aux getstream modify [eax ebx ecx edx esi edi];
void * getstream(void *fp)
{
    __db_0003b90d_0();
    __db_0003b90d_1();
    __db_0003b90d_2();
    return __db_0003b90d_3();
}
