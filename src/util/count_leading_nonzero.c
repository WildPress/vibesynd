/* BEHAVIOURALLY EQUIVALENT (verified 2026-07-21): residual = DL-vs-AL register
   allocation on the g_roster_index[param_1] byte load (ours emits an extra `mov dl,al`,
   +2B) plus two schedule transpositions (inc order; return-load vs write-back). The
   address (g_equip_kind + si*1047 + g_roster_index[param_1]*40 + i*4 -- base folded
   into disp 0xe5c3 in the target vs into a register add in ours), word compare, i<8
   bound and write-back are identical.

   frameless @ 0x20b78: for the current row g_cur_player, count the leading nonzero word
   entries (up to 8) in the strided table at 0xe5c3 for column = g_roster_index[param_1].
   Row stride 1047, column stride 40, entry stride 4 (words). Stops at the first zero
   entry and returns the count. Sibling of 0x20be8 (which uses param_1 as the column
   directly).

   NEAR-MISS (111/109; logic byte-exact). Three register-alloc/scheduler tie-breaks:
   (1) the g_roster_index[param_1] byte load lands in AL (the index reg EAX) then `mov dl,al`
   (2 bytes); the target keeps edx=si live one step longer, frees+zeroes EDX and loads
   straight into DL. (2) INC pair count(CH)/i(CL) transposed. (3) return-load vs g_cur_player
   write-back transposed. Same scheduling wall as sibling 0x20be8; the `row` temp forces
   si*1047 first (needed to get param into EAX), and `unsigned char col` shaves the
   redundant `and edx,0xff`, but the final DL-vs-AL alloc and the two transpositions are
   not source-reachable. */
extern short g_cur_player;
extern unsigned char g_equip_kind[];
extern unsigned char g_roster_index[];
unsigned char count_leading_nonzero(unsigned char param_1)
{
    short si = g_cur_player;
    unsigned char count = 0;
    unsigned char i = 0;
    do {
        int row = (int)si * 1047;
        unsigned char col = g_roster_index[param_1];
        if (*(short *)(g_equip_kind + row + (int)col * 40 + (int)i * 4) == 0)
            break;
        count++;
        i++;
    } while (i < 8);
    g_cur_player = si;
    return count;
}
