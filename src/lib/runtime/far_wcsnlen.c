/* C runtime: bounded far strlen (WIDE / 16-bit) @ 0x3bebc (CLIB3S, Watcom 9.5). Word
   sibling of 0x3be93: counts 16-bit chars of SI:[EBP+0x14], stride 2, up to [EBP+0x1c],
   stopping at a zero word or the cap. NO epilogue of its own -- both exits jz BACKWARD into
   0x3be93's shared pop/ret at 0x3beb7 (cross-function tail-merge). Full-body db of all 39
   bytes; wrapper is `aborts` so Watcom appends no RET, keeping _TEXT at 39 bytes. */
extern void __0003bebc_0(void);
#pragma aux __0003bebc_0 = "db 83" "db 86" "db 6" "db 85" "db 137" "db 229" "db 139" "db 77" "db 28" "db 102" "db 139" "db 117" "db 24" "db 139" "db 85" "db 20" "db 49" "db 192" "db 142" "db 198" "db 137" "db 211" "db 131" "db 194" "db 2" "db 102" "db 38" "db 131" "db 59" "db 0" "db 116" "db 219" "db 57" "db 200" "db 116" "db 215" "db 64" "db 235" "db 235" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux far_wcsnlen aborts modify [eax ebx ecx edx esi edi ebp];
void far_wcsnlen(void)
{
    __0003bebc_0();
}
