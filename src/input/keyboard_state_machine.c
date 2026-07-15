/* frameless @ 0x20c88: keyboard/sequence state-machine over the 0xe5b9 strided table
   (row stride 1047 = g_cur_player, column stride 40 = i). Reads current key ch = FUN_e568(0x45),
   then repeatedly scans columns i=0..17 of the current row for the byte matching ch; on a
   hit it advances ch (wrapping at 0x45) and rescans; when no column matches it writes the
   row counter back and returns ch. The `flag` local is the one-byte frame (sub esp,4); it
   is set to 1 at each outer-loop head and tested at scan-exhaustion (a compiler artifact
   that is always 1 here, matching the target's dead flag store/load).

   NEAR-MISS (byte-identical except ONE instruction). The target carries a DEAD pre-read
   `mov si,[0x10b16]` (7 bytes) before the FUN_e568 call: the row counter is loaded into the
   ESI-home register up front even though the outer-loop head (cb7) unconditionally reloads
   it, so the pre-value is never used. Our Watcom 9.5b eliminates that dead load (whole body
   otherwise matches, incl. the flag slot, the si*1047 shift chain, the i*40 SIB, the byte
   table compare, the ch wrap and both write-backs). Forcing the read back costs a register:
   `volatile` keeps it but emits the shorter `mov ax,moffs` (66 a1) accumulator form instead
   of `mov si` (66 8b 35); making it live loads it into ESI correctly but spills a 3rd reg
   (EDI push) because the live pre-value and the reloaded value can no longer share ESI. A
   dead load homed in ESI is not source-reachable here (register-role / dead-code wall). */
extern short g_cur_player;
extern unsigned char g_squad_id[];
extern unsigned char lcg_rand(int);

unsigned char keyboard_state_machine(void)
{
    unsigned char ch;
    unsigned char i;
    short si;
    unsigned char flag;

    si = g_cur_player;
    ch = lcg_rand(0x45);
    goto top;
inc_i:
    if (++i < 0x12)
        goto body;
    g_cur_player = si;
    if (flag)
        return ch;
    goto top;
top:
    flag = 1;
    si = g_cur_player;
    i = 0;
body:
    if (ch == g_squad_id[(int)si * 1047 + (int)i * 40]) {
        ++ch;
        if (ch >= 0x45)
            ch = 0;
        g_cur_player = si;
        goto top;
    }
    goto inc_i;
}
