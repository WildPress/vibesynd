/* frameless @ 0x35ed8: zero the 8 even-offset bytes (0,2,4,6,8,0xa,0xc,0xe) of
   every 0x10-byte record in the global array based at g_11bdc, for record index
   i = 0x10, 0x20, ... 0x200. The stored zero is held in a byte register (dl), so
   we keep it in a local rather than storing an immediate. */
extern unsigned char g_11bdc[];
void zero_even_bytes(void)
{
    unsigned char z = 0;
    int i = 0;
    do {
        i += 0x10;
        g_11bdc[i + 0] = z;
        g_11bdc[i + 2] = z;
        g_11bdc[i + 4] = z;
        g_11bdc[i + 6] = z;
        g_11bdc[i + 8] = z;
        g_11bdc[i + 0xa] = z;
        g_11bdc[i + 0xc] = z;
        g_11bdc[i + 0xe] = z;
    } while (i != 0x200);
}
