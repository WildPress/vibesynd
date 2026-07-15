/* @ 0x22ca8 -- init all 8 squad slots: FUN_00023158(i) for i = 0..7. */
extern void FUN_00023158(unsigned short i);

void init_squad_slots(void)
{
    unsigned short i = 0;
    do {
        FUN_00023158(i);
        i++;
    } while (i < 8);
}
