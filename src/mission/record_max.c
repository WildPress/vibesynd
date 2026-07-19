/* frameless @ 0x35f28: conditionally record a max value. If g_objective_slots[param_1*0x20 + 0x1e]
   is set, index i = param_1*2 into the paired tables g_11bec (flag) / g_11bed (value):
   if the flag is clear, store param_2 and set the flag; else if param_2 exceeds the
   stored value, overwrite it and set the flag. (g_11bec is the table 0x35ed8 clears.) */
extern unsigned char *g_objective_slots;
extern unsigned char g_11bec[];
extern unsigned char g_11bed[];
void record_max(unsigned char param_1, unsigned char param_2)
{
    if (g_objective_slots[param_1 * 0x20 + 0x1e] != 0) {
        int i = param_1 * 2;
        if (g_11bec[i] != 0) {
            if (g_11bed[i] < param_2) {
                g_11bed[i] = param_2;
                g_11bec[i] = 1;
            }
        } else {
            g_11bed[i] = param_2;
            g_11bec[i] = 1;
        }
    }
}
