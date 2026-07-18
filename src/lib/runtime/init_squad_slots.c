/* @ 0x22ca8 -- init all 8 squad slots: run_mission_command(i) for i = 0..7. */
extern void run_mission_command(unsigned short i);

void init_squad_slots(void)
{
    unsigned short i = 0;
    do {
        run_mission_command(i);
        i++;
    } while (i < 8);
}
