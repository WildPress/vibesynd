/* frameless @ 0x2c468: copy 5 fields (word,byte,word,word,word) for 8 records
   (stride 0x12) from the src table at 0x51b4 to the dst table at 0x5256, clearing
   one byte at 0xdf76 per record, then zero the global g_e11c. The record index i
   is materialised as i*9 (lea*8 + add) and scaled by 2 in each field access. */
extern unsigned char g_51b4[];
extern unsigned char g_51be[];
extern unsigned char g_51c0[];
extern unsigned char g_51c2[];
extern unsigned char g_51c4[];
extern unsigned char g_5256[];
extern unsigned char g_5260[];
extern unsigned char g_5262[];
extern unsigned char g_5264[];
extern unsigned char g_5266[];
extern unsigned char g_df76[];
extern unsigned short g_e11c;
void FUN_0002c468(void)
{
    unsigned short u = 0;
    do {
        unsigned int i = u;
        *(unsigned short *)(g_5256 + i * 0x12) = *(unsigned short *)(g_51b4 + i * 0x12);
        *(unsigned char *)(g_5260 + i * 0x12) = *(unsigned char *)(g_51be + i * 0x12);
        *(unsigned short *)(g_5262 + i * 0x12) = *(unsigned short *)(g_51c0 + i * 0x12);
        *(unsigned short *)(g_5264 + i * 0x12) = *(unsigned short *)(g_51c2 + i * 0x12);
        *(unsigned short *)(g_5266 + i * 0x12) = *(unsigned short *)(g_51c4 + i * 0x12);
        u = u + 1;
        g_df76[i] = 0;
    } while (u < 8);
    g_e11c = 0;
}
