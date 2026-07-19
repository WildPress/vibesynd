/* find_rec_by_field8 @ 0x20568 (130 bytes) -- frameless. Scan a 10-byte-record
   array until rec[8] == param_3, tracking the last matching rec[8] in `result`
   (initialised to param_3), and return it.
     - Mode param_2 == 0: when rec[9] is non-zero and indexes a still-set g_e284
       entry, clear that entry and take rec[8].
     - Mode param_2 != 0: when the cursor point (g_cursor_x, g_cursor_y) lies
       strictly inside the record's box -- x in (rec[0], rec[4]), y in
       (rec[2], rec[6]) -- take rec[8].

   MATCHED (reloc-aware YES, dist 16 = only the masked relocation bytes for the
   g_cursor_x/g_cursor_y/g_e284 addresses). Two source fixes cracked the prior
   near-miss:

   1) g_cursor_x/g_cursor_y are VOLATILE. Under -oneatx (relaxed aliasing) Watcom
      otherwise hoists both globals out of the box-test loop into callee-saved regs
      -- costing an ESI push, two head loads, two tail stores, and a spare-register
      shuffle that also spilled param_3 back to the stack each iteration. The target
      reloads each global into DX on every compare (four `mov dx, ds:...`), which is
      exactly volatile-read semantics; marking them volatile makes Watcom keep only
      EBX saved and hold param_3 in BH throughout, matching the target register map.

   2) The 1st and 3rd box comparisons put the GLOBAL on the left
      (g_cursor_x > rec[0], g_cursor_y > rec[2]) rather than the record field, so
      Watcom loads the global into DX and compares against the memory operand
      (`mov dx,global; cmp dx,[eax]; jbe`) -- the target's operand order and branch
      sense -- instead of loading the field and comparing against the global. */
extern unsigned char g_e284[];
extern volatile unsigned short g_cursor_x, g_cursor_y;
unsigned char find_rec_by_field8(unsigned char *recs, unsigned char param_2, unsigned char param_3)
{
    unsigned char result = param_3;
    if (param_2 == 0) {
        while (recs[8] != param_3) {
            if (recs[9] != 0 && g_e284[recs[9]] != 0) {
                g_e284[recs[9]] = 0;
                result = recs[8];
            }
            recs += 0xa;
        }
    } else {
        while (recs[8] != param_3) {
            if (g_cursor_x > *(unsigned short *)recs &&
                g_cursor_x < *(unsigned short *)(recs + 4) &&
                g_cursor_y > *(unsigned short *)(recs + 2) &&
                g_cursor_y < *(unsigned short *)(recs + 6))
                result = recs[8];
            recs += 0xa;
        }
    }
    return result;
}
