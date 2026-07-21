/* prefix @ 0xdc08 -- guarded one-shot state setter (previously-undecoded render-path prefix).
 * If neither bit 1/2 of flag g_in_mission is set: stamp g_status_blink=0x10, set bit 2 of g_in_mission, copy
 * g_10ad8->g_c34c, g_c354=1, and call record_max(0x18,0x7f). Frameless, no params.
 * Recipe: -4s -oneatx -zp8 -s -zq.
 * PARKED near-miss (~95%, 3 bytes): target stores the 16-bit const directly (mov word [g_status_blink],0x10),
 * Watcom routes ours through EDX (mov edx,0x10; mov [g_status_blink],dx) -- a constant-materialization codegen
 * tie; permuter (3000 variants incl. pad/annealing) didn't flip it. Logic byte-correct. */
extern unsigned char g_in_mission;
extern unsigned short g_status_blink;
extern int g_10ad8, g_c34c, g_c354;
extern void record_max(int a, int b);

void oneshot_state_setter(void)
{
    if ((g_in_mission & 6) == 0) {
        g_status_blink = 0x10;
        g_in_mission |= 4;
        g_c34c = g_10ad8;
        g_c354 = 1;
        record_max(0x18, 0x7f);
    }
}
