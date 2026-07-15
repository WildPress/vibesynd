/* frameless @ 0x1ba48: clamp a point (param_1,param_2) into the bounding box
   [g_1be32,g_1be36] x [g_1be34,g_1be38], snap to even (& 0xfe), store to g_map_cursor_x/g_map_cursor_y,
   reset accumulators g_marker_phase_a/g_marker_phase_b, then draw a 0x19-step diagonal via 25 calls to
   FUN_00045f8a(x+i, y+i, 0x10, g_marker_phase_b, i); finish with fill_minimap_grid().
   NOTE: manifest size (114B, ends at 0x1baba loop-top) under-counts; true extent is
   0x1ba48-0x1baf2 = 171 bytes. */
extern short g_1be32, g_1be34, g_1be36, g_1be38;
extern int g_marker_phase_a, g_marker_phase_b;
extern unsigned short g_map_cursor_x, g_map_cursor_y;
extern int FUN_00045f8a();
extern void fill_minimap_grid(void);

void clamp_point_box(short param_1, short param_2)
{
    unsigned short i;
    g_marker_phase_b = 0;
    g_marker_phase_a = 0;
    if (param_1 > g_1be36) param_1 = g_1be36;
    if (param_2 > g_1be38) param_2 = g_1be38;
    if (param_1 < g_1be32) param_1 = g_1be32;
    if (param_2 < g_1be34) param_2 = g_1be34;
    g_map_cursor_x = param_1 & 0xfe;
    g_map_cursor_y = param_2 & 0xfe;
    for (i = 0; i < 0x19; i++)
        FUN_00045f8a((short)(g_map_cursor_x + i), (short)(g_map_cursor_y + i), 0x10, g_marker_phase_b, i);
    fill_minimap_grid();
}
