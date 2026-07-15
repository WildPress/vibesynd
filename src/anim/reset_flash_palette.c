/* frameless @ 0x13ac8: reset obj1 state and flash obj2's palette entry twice. Zero
   obj1[0xa](word)/[2]/[5], copy obj1[4]->obj1[3]; set obj2[0xd]=0x14 and obj2's three
   words [0]/[1]/[2] to 0x7f, call FUN_00023158(param_3); set obj2[0xd]=0xb and call
   again; if obj1[3] became 2, set it to 8. */
extern void FUN_00023158(unsigned int);
void reset_flash_palette(unsigned char *obj1, unsigned short *obj2, unsigned char param_3)
{
    unsigned char t;
    *(unsigned short *)(obj1 + 0xa) = 0;
    obj1[2] = 0;
    t = obj1[4];
    obj1[5] = 0;
    obj1[3] = t;
    *((unsigned char *)obj2 + 0xd) = 0x14;
    obj2[0] = 0x7f;
    obj2[1] = 0x7f;
    obj2[2] = 0x7f;
    FUN_00023158((unsigned short)param_3);
    *((unsigned char *)obj2 + 0xd) = 0xb;
    FUN_00023158((unsigned short)param_3);
    if (obj1[3] == 2)
        obj1[3] = 8;
}
