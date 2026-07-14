/* C runtime: switch-char/cmd-prefix helper @ 0x3cbf9 (CLIB3S; heuristic mislabeled spawnve).
   If arg3!=0 store '/'; else DOS get-switch-char (AH=0x37); then append 'c',0. Returns buf.
   Pure asm (int21, no call) -- full body via #pragma aux db bytes. */
extern char *__swch(void);
#pragma aux __swch = "db 139" "db 93" "db 12" "db 128" "db 125" "db 16" "db 0" "db 116" "db 5" "db 198" "db 3" "db 47" "db 235" "db 14" "db 82" "db 180" "db 55" "db 176" "db 0" "db 205" "db 33" "db 136" "db 208" "db 180" "db 0" "db 90" "db 136" "db 3" "db 198" "db 67" "db 1" "db 99" "db 198" "db 67" "db 2" "db 0" "db 137" "db 216" parm [] value [eax] modify exact [eax ebx edx];
char *FUN_0003cbf9(char *buf, int p2, int flag)
{
    return __swch();
}
