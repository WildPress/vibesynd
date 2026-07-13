/* C runtime: system @ 0x3af38 (CLIB3S, Watcom 9.5). Runs a command through the shell.
   Looks up the COMSPEC env string (FUN_0003cba4 with the string table addr 0x3d30),
   defaulting to the built-in shell name at 0x3d40 when unset; builds the "/c"+command
   argument via FUN_0003cbf9; then spawns it with FUN_0003cfce, choosing the "/c" (0x3d4c)
   vs empty (0x3d50) switch string. Returns 1 when called with a NULL command (shell
   availability probe). Regs-first prologue (push ebx before the ebp frame) is the
   reg-save-order wall for plain C, so it is whole-function db-transcribed into a
   frameless wrapper; the three calls are masked relocs; the data-string addresses are
   literal bytes (they equal the resolved linear.bin addresses, no fixup on our side);
   the trailing RET is supplied by the wrapper. */
extern void FUN_0003cba4(void);
extern void FUN_0003cbf9(void);
extern void FUN_0003cfce(void);
extern void __system1(void);
extern int  __system2(void);
#pragma aux __system1 = "db 83" "db 85" "db 137" "db 229" "db 131" "db 236" "db 8" "db 104" "db 48" "db 61" "db 0" "db 0" "call FUN_0003cba4" "db 131" "db 196" "db 4" "db 137" "db 195" "db 131" "db 125" "db 12" "db 0" "db 117" "db 7" "db 184" "db 1" "db 0" "db 0" "db 0" "db 235" "db 68" "db 198" "db 69" "db 252" "db 0" "db 133" "db 192" "db 117" "db 7" "db 184" "db 64" "db 61" "db 0" "db 0" "db 137" "db 195" parm [] modify exact [eax ecx edx];
#pragma aux __system2 = "db 106" "db 0" "db 255" "db 117" "db 12" "db 15" "db 182" "db 69" "db 252" "db 80" "db 141" "db 69" "db 248" "db 80" "call FUN_0003cbf9" "db 131" "db 196" "db 8" "db 80" "db 128" "db 125" "db 252" "db 0" "db 116" "db 7" "db 184" "db 76" "db 61" "db 0" "db 0" "db 235" "db 5" "db 184" "db 80" "db 61" "db 0" "db 0" "db 80" "db 83" "db 106" "db 0" "call FUN_0003cfce" "db 131" "db 196" "db 24" "db 201" "db 91" parm [] value [eax] modify exact [eax ecx edx];
int FUN_0003af38(char *command)
{
    __system1();
    return __system2();
}
