/* frameless @ 0x14998: walk a chain of 15-byte records at g_1beb2, following
   the link at record+2, until the link is zero. (do-while form) */
extern unsigned char g_1beb2[];
void walk_15byte_chain(unsigned char *p)
{
    if (*(unsigned short *)(p + 2) != 0) {
        do {
            p = g_1beb2 + *(unsigned short *)(p + 2) * 15;
        } while (*(unsigned short *)(p + 2) != 0);
    }
}
