/*
 * FUN_0002d3b8 - commit/advance the shot-cursor accumulator.
 *
 * Validates the current shot-cursor position (g_aim_x, g_aim_y, g_aim_level)
 * against the entity param_1 via FUN_0002d468. If valid, it advances the
 * z/height accumulator g_aim_level by the step g_level_step, stores it back, and
 * commits the new cursor via FUN_00026c78, returning 0 (success).
 * If the position is not valid, it leaves the cursor unchanged and
 * returns 1 (failure).
 */

extern short g_aim_x;
extern short g_aim_y;
extern short g_aim_level;
extern short g_level_step;

extern short FUN_0002d468(int, int, int, int);
extern int FUN_00026c78(int, int, int, int);

int FUN_0002d3b8(int param_1)
{
    if (FUN_0002d468(g_aim_x, g_aim_y, g_aim_level, param_1) != 0) {
        g_aim_level = g_aim_level + g_level_step;
        FUN_00026c78(param_1, g_aim_x, g_aim_y, g_aim_level);
        return 0;
    }
    return 1;
}
