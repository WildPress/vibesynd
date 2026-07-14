/* C runtime: segread @ 0x3b3b9 (CLIB3S). Reads CS/DS/ES/SS/FS/GS into *sregs.
   Emitted as raw db bytes via #pragma aux (Watcom mini-asm rejects mov eax,Sreg). */
extern void __segread(void *sregs);
#pragma aux __segread = "db 140" "db 200" "db 102" "db 137" "db 67" "db 2" "db 140" "db 216" "db 102" "db 137" "db 67" "db 6" "db 140" "db 192" "db 102" "db 137" "db 3" "db 140" "db 208" "db 102" "db 137" "db 67" "db 4" "db 140" "db 224" "db 102" "db 137" "db 67" "db 8" "db 140" "db 232" "db 102" "db 137" "db 67" "db 10" parm [ebx] modify exact [eax];
void segread(void *sregs)
{
    __segread(sregs);
}
