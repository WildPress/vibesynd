/*
 * FUN_0002d3b8 - commit/advance the shot-cursor accumulator.
 *
 * Validates the current shot-cursor position (g_10b28, g_10b2a, g_10b2c)
 * against the entity param_1 via FUN_0002d468. If valid, it advances the
 * z/height accumulator g_10b2c by the step g_e128, stores it back, and
 * commits the new cursor via FUN_00026c78, returning 0 (success).
 * If the position is not valid, it leaves the cursor unchanged and
 * returns 1 (failure).
 */

extern short g_10b28;
extern short g_10b2a;
extern short g_10b2c;
extern short g_e128;

extern short FUN_0002d468(int, int, int, int);
extern int FUN_00026c78(int, int, int, int);

int FUN_0002d3b8(int param_1)
{
    if (FUN_0002d468(g_10b28, g_10b2a, g_10b2c, param_1) != 0) {
        g_10b2c = g_10b2c + g_e128;
        FUN_00026c78(param_1, g_10b28, g_10b2a, g_10b2c);
        return 0;
    }
    return 1;
}
