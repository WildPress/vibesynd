/* frameless @ 0x14998: walk a chain of 15-byte records at g_1beb2, following
   the link at record+2, until the link is zero. */
extern unsigned char g_1beb2[];
void FUN_00014998(unsigned char *p)
{
    while (*(unsigned short *)(p + 2) != 0)
        p = g_1beb2 + *(unsigned short *)(p + 2) * 15;
}
