/* C runtime: d_getvec @ 0x3b239 (CLIB3S). DOS get-interrupt-vector (AH=0x35, with a
   DOS-version gate on the 0xc2da flag). int21 asm replicated via #pragma aux (db bytes);
   the 0xc2da refs are literals matching the resolved flag address. */
extern int __d_getvec(int intno);
#pragma aux __d_getvec = "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 2" "db 114" "db 17" "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 8" "db 119" "db 8" "db 102" "db 184" "db 2" "db 37" "db 136" "db 217" "db 235" "db 14" "db 129" "db 227" "db 255" "db 0" "db 0" "db 0" "db 128" "db 207" "db 53" "db 48" "db 201" "db 102" "db 137" "db 216" "db 6" "db 205" "db 33" "db 140" "db 194" "db 7" "db 137" "db 216" parm [ebx] value [eax] modify exact [eax ecx edx];
int FUN_0003b239(int intno)
{
    return __d_getvec(intno);
}
