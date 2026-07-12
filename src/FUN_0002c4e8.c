extern unsigned short g_e11c;
extern unsigned char g_df76[];
void FUN_0002c4e8(void)
{
    unsigned short u = 0;
    do {
        unsigned int w = u;
        int i = w * 0x12;
        if (*(short *)(i + 0x5256) != 2 && *(short *)(i + 0x5256) != 3) {
            *(unsigned short *)(i + 0x5256) = *(unsigned short *)(i + 0x51b4);
            *(unsigned char  *)(i + 0x5260) = *(unsigned char  *)(i + 0x51be);
            *(unsigned short *)(i + 0x5262) = *(unsigned short *)(i + 0x51c0);
            *(unsigned short *)(i + 0x5264) = *(unsigned short *)(i + 0x51c2);
            *(unsigned short *)(i + 0x5266) = *(unsigned short *)(i + 0x51c4);
            g_df76[w] = 0;
        }
        u = u + 1;
    } while (u < 8);
    g_e11c = 0;
}
