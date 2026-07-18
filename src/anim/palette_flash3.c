/* frameless @ 0x13b38: three-stage palette flash (sibling of 0x13ac8). Clear param_5[5];
   then three times set obj1[0xd] to a stage id, fill obj1's three words, and call
   run_mission_command(param_4): stage 7 uses param_2[0x3a] for obj1[0]; stage 0x14 uses the
   constant 0x7f/0x7f/0xff; stage 8 uses param_3[4]/[6]/[8]+0x80. */
extern void run_mission_command(unsigned int);
void palette_flash3(short *obj1, int param_2, int param_3, unsigned char param_4, unsigned char *param_5)
{
    param_5[5] = 0;
    *((unsigned char *)obj1 + 0xd) = 7;
    obj1[0] = *(short *)(param_2 + 0x3a);
    run_mission_command((unsigned short)param_4);
    *((unsigned char *)obj1 + 0xd) = 0x14;
    obj1[0] = 0x7f;
    obj1[1] = 0x7f;
    obj1[2] = 0xff;
    run_mission_command((unsigned short)param_4);
    *((unsigned char *)obj1 + 0xd) = 8;
    obj1[0] = *(short *)(param_3 + 4);
    obj1[1] = *(short *)(param_3 + 6);
    obj1[2] = *(short *)(param_3 + 8) + 0x80;
    run_mission_command((unsigned short)param_4);
}
