/* frameless @ 0x20568: search a 10-byte-record array until rec[8] == param_3, tracking
   the last matching rec[8] in `result` (initialised to param_3). Mode param_2==0: when
   rec[9] indexes a set g_e284 entry, clear it and take rec[8]. Mode param_2!=0: when the
   point (g_cursor_x, g_cursor_y) lies strictly inside the record's box (rec[0]/[4] x, rec[2]/[6]
   y), take rec[8]. Return the byte result.

   PARKED near-miss (NOT matched; logic correct). Loop-invariant-hoisting + register wall:
   -oneatx hoists the globals g_cursor_x/g_cursor_y out of the box-test loop into callee-saved
   regs (adds a push and wastes them in the param_2==0 path); the target reloads them into
   DX each compare and fits everything in EAX/EBX/ECX/EDX (only EBX saved). No recipe
   (-ot/-oat/-os/-oa/-oe) reproduces the tight-register + per-iteration-reload profile.
   Same class as 0x26da8 / 0x269d8. */
extern unsigned char g_e284[];
extern unsigned short g_cursor_x, g_cursor_y;
unsigned char FUN_00020568(unsigned char *recs, unsigned char param_2, unsigned char param_3)
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
            if (*(unsigned short *)recs < g_cursor_x &&
                g_cursor_x < *(unsigned short *)(recs + 4) &&
                *(unsigned short *)(recs + 2) < g_cursor_y &&
                g_cursor_y < *(unsigned short *)(recs + 6))
                result = recs[8];
            recs += 0xa;
        }
    }
    return result;
}
