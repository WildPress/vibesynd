/* framed field-mask+call @ 0x3b407: p=[ebp+8]; p[0xc]&=0xcf; g(p,0,0) */
extern void fseek(unsigned char *p, int y, int z);
void rewind(unsigned char *p)
{
    p[0xc] &= 0xcf;
    fseek(p, 0, 0);
}
