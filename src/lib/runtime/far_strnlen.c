/* C runtime: bounded far strlen (byte) @ 0x3be93 (CLIB3S, Watcom 9.5). Counts bytes of
   the far string SI:[EBP+0x14] up to max [EBP+0x1c], stopping at NUL or cap; count in EAX.
   Frame push ebx;esi;es;ebp; mov ebp,esp; epilogue pop ebp;es;esi;ebx. Full-body db, RET dropped. */
extern void __0003be93_0(void);
#pragma aux __0003be93_0 = "db 83" "db 86" "db 6" "db 85" "db 137" "db 229" "db 102" "db 139" "db 117" "db 24" "db 139" "db 85" "db 20" "db 139" "db 77" "db 28" "db 49" "db 192" "db 142" "db 198" "db 137" "db 211" "db 66" "db 38" "db 128" "db 59" "db 0" "db 116" "db 7" "db 57" "db 200" "db 116" "db 3" "db 64" "db 235" "db 238" "db 93" "db 7" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux far_strnlen modify [eax ebx ecx edx esi edi ebp];
void far_strnlen(void)
{
    __0003be93_0();
}
