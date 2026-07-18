/* framed field-mask+call @ 0x3b407: p=[ebp+8]; p[0xc]&=0xcf; g(p,0,0) */
extern void FUN_0003b594(unsigned char *p, int y, int z);
void rewind(unsigned char *p)
{
    p[0xc] &= 0xcf;
    FUN_0003b594(p, 0, 0);
}
