/* C runtime: d_setvec @ 0x3b273 (CLIB3S). DOS set-interrupt-vector (AH=0x25) with the
   DOS-version gate. Entire body (incl the [ebp+N] param loads -- the segment loads 16-bit
   via mov cx) emitted as #pragma aux db bytes; -d2 supplies the ebp frame. */
extern void __d_setvec(void);
#pragma aux __d_setvec = "db 139" "db 69" "db 8" "db 102" "db 139" "db 77" "db 16" "db 139" "db 85" "db 12" "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 2" "db 114" "db 23" "db 128" "db 61" "db 218" "db 194" "db 0" "db 0" "db 8" "db 119" "db 14" "db 30" "db 142" "db 217" "db 136" "db 193" "db 176" "db 4" "db 180" "db 37" "db 205" "db 33" "db 31" "db 235" "db 8" "db 30" "db 142" "db 217" "db 180" "db 37" "db 205" "db 33" "db 31" parm [] modify exact [eax ecx edx];
void d_setvec(int a, int b, short c)
{
    __d_setvec();
}
