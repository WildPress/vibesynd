/* C runtime: system() / spawn-command @ 0x3cc74 (CLIB3S, Watcom 9.5). Large framed
   hand-asm: stack-probe prologue (push 0xd8; call stk_check_thunk), saves ebx/esi/edi + ES,
   ENTER 0xa4,0 then SUB EBP,0x5a for a big local frame. Parses/splits the command line
   (build_exec_block), builds argv and the environment block, alloca's scratch via the
   stack_avail/SUB ESP stack-carve idiom, resolves COMSPEC and the /c switches (literal
   string ptrs 0x3d58.. via stricmp/strcpy/getenv), spawns through the
   loader spawn_exec/spawn_load_check, sets errno (errno_ptr/doserrno_ptr) on failure, and
   frees everything (free/nfree) before returning the child status in ESI.
   Two indirect calls go through the global vtable slots [0xc324]/[0xc328] (literal FF15
   db). Whole body db-transcribed into a frameless wrapper; all direct CALL rel32s are
   masked externs, every string/global address is a literal db abs32, and the segment
   ops (push ES/pop ES, movsd/movsb) are literal bytes; trailing RET supplied by wrapper. */
extern void stk_check_thunk(void);
extern void errno_ptr(void);
extern void build_exec_block(void);
extern void strlen(void);
extern void nmalloc(void);
extern void stack_avail(void);
extern void free(void);
extern void splitpath2(void);
extern void doserrno_ptr(void);
extern void FUN_0003e381(void);
extern void stricmp(void);
extern void spawn_load_check(void);
extern void spawn_exec(void);
extern void strcpy(void);
extern void build_cmdtail(void);
extern void spawnve(void);
extern void getenv(void);
extern void spawn_with_env(void);
extern void nfree(void);
extern void __FUN_0003cc74_0(void);
extern void __FUN_0003cc74_1(void);
extern void __FUN_0003cc74_2(void);
extern void __FUN_0003cc74_3(void);
extern void __FUN_0003cc74_4(void);
extern void __FUN_0003cc74_5(void);
extern void __FUN_0003cc74_6(void);
extern void __FUN_0003cc74_7(void);
extern void __FUN_0003cc74_8(void);
extern void __FUN_0003cc74_9(void);
extern void __FUN_0003cc74_10(void);
extern void __FUN_0003cc74_11(void);
#pragma aux __FUN_0003cc74_0 = "db 104" "db 216" "db 0" "db 0" "db 0" "call stk_check_thunk" "db 83" "db 86" "db 87" "db 6" "db 200" "db 164" "db 0" "db 0" "db 131" "db 237" "db 90" "db 198" "db 69" "db 86" "db 0" "db 131" "db 125" "db 114" "db 2" "db 114" "db 21" "call errno_ptr" "db 199" "db 0" "db 9" "db 0" "db 0" "db 0" "db 184" "db 255" "db 255" "db 255" "db 255" "db 233" "db 29" "db 3" "db 0" "db 0" "db 106" "db 0" "db 141" "db 69" "db 62" "db 80" "db 141" "db 69" "db 74" "db 80" "db 141" "db 69" "db 78" "db 80" "db 141" "db 69" "db 82" "db 80" "db 255" "db 117" "db 126" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_1 = "db 255" "db 117" "db 122" "call build_exec_block" "db 131" "db 196" "db 28" "db 137" "db 198" "db 131" "db 248" "db 255" "db 15" "db 132" "db 242" "db 2" "db 0" "db 0" "db 255" "db 117" "db 118" "call strlen" "db 131" "db 196" "db 4" "db 141" "db 184" "db 154" "db 0" "db 0" "db 0" "db 87" "call nmalloc" "db 131" "db 196" "db 4" "db 137" "db 69" "db 70" "db 133" "db 192" "db 117" "db 47" "db 141" "db 95" "db 3" "db 128" "db 227" "db 252" "call stack_avail" "db 57" "db 195" "db 115" "db 8" "db 137" "db 216" "db 41" "db 196" "db 137" "db 224" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_2 = "db 235" "db 2" "db 49" "db 192" "db 137" "db 195" "db 133" "db 192" "db 117" "db 18" "db 255" "db 117" "db 82" "call free" "db 131" "db 196" "db 4" "db 233" "db 123" "db 255" "db 255" "db 255" "db 137" "db 195" "db 141" "db 69" "db 34" "db 80" "db 141" "db 69" "db 38" "db 80" "db 141" "db 69" "db 42" "db 80" "db 141" "db 69" "db 46" "db 80" "db 129" "db 239" "db 147" "db 0" "db 0" "db 0" "db 1" "db 223" "db 87" "db 255" "db 117" "db 118" "call splitpath2" "db 131" "db 196" "db 24" "db 141" "db 69" "db 182" "db 80" "db 255" "db 21" "db 36" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_3 = "db 195" "db 0" "db 0" "db 131" "db 196" "db 4" "db 255" "db 117" "db 62" "call nmalloc" "db 131" "db 196" "db 4" "db 137" "db 69" "db 58" "db 133" "db 192" "db 117" "db 74" "db 139" "db 125" "db 62" "db 131" "db 199" "db 3" "db 102" "db 131" "db 231" "db 252" "call stack_avail" "db 57" "db 199" "db 115" "db 14" "db 139" "db 69" "db 62" "db 131" "db 192" "db 3" "db 36" "db 252" "db 41" "db 196" "db 137" "db 224" "db 235" "db 2" "db 49" "db 192" "db 137" "db 69" "db 54" "db 133" "db 192" "db 117" "db 35" "db 190" "db 255" "db 255" "db 255" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_4 = "call errno_ptr" "db 199" "db 0" "db 2" "db 0" "db 0" "db 0" "call doserrno_ptr" "db 199" "db 0" "db 10" "db 0" "db 0" "db 0" "db 233" "db 227" "db 1" "db 0" "db 0" "db 137" "db 69" "db 54" "db 128" "db 61" "db 226" "db 194" "db 0" "db 0" "db 3" "db 114" "db 23" "db 139" "db 69" "db 46" "db 128" "db 56" "db 0" "db 117" "db 15" "db 139" "db 69" "db 42" "db 128" "db 56" "db 0" "db 117" "db 7" "db 199" "db 69" "db 42" "db 88" "db 61" "db 0" "db 0" "db 255" "db 117" "db 34" "db 255" "db 117" "db 38" "db 255" "db 117" "db 42" "db 255" "db 117" "db 46" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_5 = "db 83" "call FUN_0003e381" "db 131" "db 196" "db 20" "call errno_ptr" "db 199" "db 0" "db 1" "db 0" "db 0" "db 0" "db 139" "db 69" "db 34" "db 128" "db 56" "db 0" "db 116" "db 98" "db 128" "db 125" "db 86" "db 0" "db 116" "db 7" "db 184" "db 91" "db 61" "db 0" "db 0" "db 235" "db 5" "db 184" "db 96" "db 61" "db 0" "db 0" "db 80" "db 255" "db 117" "db 34" "call stricmp" "db 131" "db 196" "db 8" "db 133" "db 192" "db 117" "db 27" "db 190" "db 255" "db 255" "db 255" "db 255" "db 83" "call spawn_load_check" "db 131" "db 196" "db 4" "db 133" "db 192" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_6 = "db 15" "db 133" "db 247" "db 0" "db 0" "db 0" "db 233" "db 91" "db 1" "db 0" "db 0" "call errno_ptr" "db 199" "db 0" "db 0" "db 0" "db 0" "db 0" "db 255" "db 117" "db 122" "db 255" "db 117" "db 82" "db 255" "db 117" "db 54" "db 83" "db 255" "db 117" "db 114" "call spawn_exec" "db 131" "db 196" "db 20" "db 233" "db 52" "db 1" "db 0" "db 0" "db 83" "call strlen" "db 131" "db 196" "db 4" "db 141" "db 60" "db 3" "db 137" "db 125" "db 66" "db 128" "db 125" "db 86" "db 0" "db 116" "db 13" "call errno_ptr" "db 199" "db 0" "db 1" "db 0" "db 0" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_7 = "db 0" "db 235" "db 43" "db 30" "db 7" "db 190" "db 101" "db 61" "db 0" "db 0" "db 165" "db 164" "call errno_ptr" "db 199" "db 0" "db 0" "db 0" "db 0" "db 0" "db 255" "db 117" "db 122" "db 255" "db 117" "db 82" "db 255" "db 117" "db 54" "db 83" "db 255" "db 117" "db 114" "call spawn_exec" "db 131" "db 196" "db 20" "db 137" "db 198" "call errno_ptr" "db 131" "db 56" "db 1" "db 15" "db 133" "db 219" "db 0" "db 0" "db 0" "call errno_ptr" "db 199" "db 0" "db 0" "db 0" "db 0" "db 0" "db 30" "db 7" "db 139" "db 125" "db 66" "db 190" "db 106" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_8 = "db 61" "db 0" "db 0" "db 165" "db 164" "db 255" "db 117" "db 122" "db 255" "db 117" "db 82" "db 255" "db 117" "db 54" "db 83" "db 255" "db 117" "db 114" "call spawn_exec" "db 131" "db 196" "db 20" "db 137" "db 198" "call errno_ptr" "db 131" "db 56" "db 1" "db 15" "db 133" "db 159" "db 0" "db 0" "db 0" "call errno_ptr" "db 199" "db 0" "db 0" "db 0" "db 0" "db 0" "db 128" "db 125" "db 86" "db 0" "db 116" "db 7" "db 184" "db 91" "db 61" "db 0" "db 0" "db 235" "db 5" "db 184" "db 96" "db 61" "db 0" "db 0" "db 80" "db 255" "db 117" "db 66" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_9 = "call strcpy" "db 131" "db 196" "db 8" "db 83" "call spawn_load_check" "db 131" "db 196" "db 4" "db 133" "db 192" "db 116" "db 105" "db 255" "db 117" "db 82" "call free" "db 131" "db 196" "db 4" "db 199" "db 69" "db 82" "db 0" "db 0" "db 0" "db 0" "db 106" "db 1" "db 255" "db 117" "db 54" "db 255" "db 117" "db 122" "db 83" "call build_cmdtail" "db 131" "db 196" "db 16" "db 106" "db 0" "db 255" "db 117" "db 54" "db 83" "db 15" "db 182" "db 69" "db 86" "db 80" "db 141" "db 69" "db 50" "db 80" "call spawnve" "db 131" "db 196" "db 8" "db 80" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_10 = "db 128" "db 125" "db 86" "db 0" "db 116" "db 7" "db 184" "db 119" "db 61" "db 0" "db 0" "db 235" "db 5" "db 184" "db 123" "db 61" "db 0" "db 0" "db 80" "db 104" "db 111" "db 61" "db 0" "db 0" "call getenv" "db 131" "db 196" "db 4" "db 80" "db 255" "db 117" "db 114" "call spawn_with_env" "db 131" "db 196" "db 28" "db 137" "db 198" "db 255" "db 117" "db 58" "call nfree" "db 131" "db 196" "db 4" "db 255" "db 117" "db 70" "call nfree" "db 131" "db 196" "db 4" "db 255" "db 117" "db 82" "call free" "db 131" "db 196" "db 4" "db 141" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003cc74_11 = "db 69" "db 182" "db 80" "db 255" "db 21" "db 40" "db 195" "db 0" "db 0" "db 131" "db 196" "db 4" "db 137" "db 240" "db 141" "db 101" "db 90" "db 93" "db 7" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003cc74 modify [eax ebx ecx edx esi edi ebp];
void FUN_0003cc74(void)
{
    __FUN_0003cc74_0();
    __FUN_0003cc74_1();
    __FUN_0003cc74_2();
    __FUN_0003cc74_3();
    __FUN_0003cc74_4();
    __FUN_0003cc74_5();
    __FUN_0003cc74_6();
    __FUN_0003cc74_7();
    __FUN_0003cc74_8();
    __FUN_0003cc74_9();
    __FUN_0003cc74_10();
    __FUN_0003cc74_11();
}
