/* constant setter @ 0x0003e4aa (11B): mov dword[0xc2f0],0x8000 ; ret */
extern int g_c2f0;
void set_heap_alloc_flag(void)
{
    g_c2f0 = 0x8000;
}
