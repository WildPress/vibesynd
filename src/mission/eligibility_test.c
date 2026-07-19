/* frameless @ 0x146f8: eligibility test, returns 1 unless a guard fails. Return 0 if
   (obj2[5] != 0xff and obj1[7] != obj2[5]); or (obj2[7] == 3 and obj2[9] == obj1[7]);
   or (obj2[6] & 8 and walk_chain_3a(obj1[0xc], 0xc) == 0). Otherwise return 1. */
extern short walk_chain_3a(int, int);
char eligibility_test(unsigned char *obj1, unsigned char *obj2)
{
    if (obj2[5] != 0xff && obj1[7] != obj2[5])
        return 0;
    if (obj2[7] == 3 && *(unsigned short *)(obj2 + 9) == obj1[7])
        return 0;
    if ((obj2[6] & 8) && walk_chain_3a(*(int *)(obj1 + 0xc), 0xc) == 0)
        return 0;
    return 1;
}
