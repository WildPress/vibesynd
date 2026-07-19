/* frameless linear call-seq @ 0x25168: g(ptr) five times with fixed pointers */
extern void printf(int p);
void init_call5(void)
{
    printf(0x35a8);
    printf(0x35c4);
    printf(0x35ec);
    printf(0x3614);
    printf(0x363c);
}
