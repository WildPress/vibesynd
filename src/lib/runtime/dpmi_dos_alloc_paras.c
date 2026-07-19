/* C runtime: DPMI/DOS paragraph allocator @ 0x3c772 (CLIB3S, Watcom 9.5). Framed
   (push ebx/esi/edi/ebp; mov ebp,esp). Splits the byte size arg [ebp+0x14] into
   segment:offset, tries a DPMI "resize memory block" (INT 31h AX=0x0501); on failure
   it falls back to raw DOS: clamps the DPMI-largest-block record at g_11e50/g_11e52,
   allocates paragraphs via INT 21h AH=0x48, on a >64K request also allocates the second
   block and INT 21h AH=0x49 frees the first through ES, then INT 31h AX=0x0006 to read
   the linear base, returning the linear address (selector<<16|dx). Returns 0 on any
   failure. No external CALLs (pure INT 21h/31h); whole body db-transcribed into a
   frameless wrapper, g_11e50/g_11e52 literal abs32s, segment ops literal; RET from wrapper. */
extern void __FUN_0003c772_0(void);
extern void __FUN_0003c772_1(void);
extern void __FUN_0003c772_2(void);
#pragma aux __FUN_0003c772_0 = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 69" "db 20" "db 193" "db 232" "db 16" "db 102" "db 139" "db 77" "db 20" "db 102" "db 137" "db 195" "db 180" "db 5" "db 176" "db 1" "db 205" "db 49" "db 25" "db 192" "db 247" "db 208" "db 117" "db 9" "db 102" "db 137" "db 216" "db 193" "db 224" "db 16" "db 102" "db 137" "db 200" "db 133" "db 192" "db 15" "db 133" "db 111" "db 0" "db 0" "db 0" "db 102" "db 247" "db 5" "db 82" "db 30" "db 1" "db 0" "db 240" "db 255" "db 116" "db 10" "db 199" "db 5" "db 80" "db 30" "db 1" "db 0" "db 255"  modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003c772_1 = "db 255" "db 15" "db 0" "db 129" "db 125" "db 20" "db 0" "db 0" "db 1" "db 0" "db 119" "db 79" "db 139" "db 29" "db 80" "db 30" "db 1" "db 0" "db 193" "db 235" "db 4" "db 128" "db 203" "db 1" "db 180" "db 72" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 198" "db 133" "db 192" "db 124" "db 53" "db 139" "db 93" "db 20" "db 193" "db 235" "db 4" "db 180" "db 72" "db 205" "db 33" "db 209" "db 208" "db 209" "db 200" "db 137" "db 199" "db 137" "db 195" "db 137" "db 240" "db 6" "db 142" "db 192" "db 180" "db 73" "db 205" "db 33" "db 209"  modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __FUN_0003c772_2 = "db 208" "db 209" "db 200" "db 7" "db 133" "db 255" "db 124" "db 17" "db 180" "db 0" "db 176" "db 6" "db 205" "db 49" "db 102" "db 137" "db 200" "db 193" "db 224" "db 16" "db 102" "db 139" "db 194" "db 235" "db 2" "db 49" "db 192" "db 93" "db 95" "db 94" "db 91"  modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux dpmi_dos_alloc_paras modify [eax ebx ecx edx esi edi ebp];
void dpmi_dos_alloc_paras(void)
{
    __FUN_0003c772_0();
    __FUN_0003c772_1();
    __FUN_0003c772_2();
}
