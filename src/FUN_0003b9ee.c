/* Decomp target: nibble (0-15) -> ASCII hex digit.  original @ 0x0003b9ee (19 bytes)
 *
 *   push ebp; mov ebp,esp
 *   mov eax,[ebp+8]      ; stack param
 *   add eax,0x30         ; '0'
 *   cmp eax,0x39         ; '9'
 *   jle +3
 *   add eax,0x27         ; -> 'a'..'f'
 *   pop ebp; ret
 *
 * Param at [ebp+8] => STACK calling convention. Calibration target for flags.
 */
int FUN_0003b9ee(int n)
{
    n += 0x30;
    if (n > 0x39)
        n += 0x27;
    return n;
}
