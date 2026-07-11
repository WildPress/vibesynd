/* frameless @ 0x253a8: for each 6-byte record in [start,end), if global mode byte
   g_105 is 2 or 4, add (arg3+0x14) to the record dword and set record[5]=0x20. */
extern unsigned char g_105;
void FUN_000253a8(unsigned char *start, unsigned char *end, int arg3)
{
    while (start < end) {
        switch (g_105) {
        case 4:
            *(int *)start += arg3 + 0x14;
            start[5] = 0x20;
            break;
        case 2:
            *(int *)start += arg3 + 0x14;
            start[5] = 0x20;
            break;
        }
        start += 6;
    }
}
