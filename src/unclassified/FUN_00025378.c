/* frameless push-ebx loop @ 0x25378: for(p=a; p<b; p+=6bytes) *p += c */
void FUN_00025378(int *a, int *b, int c)
{
    while (a < b) {
        *a += c;
        a = (int *)((char *)a + 6);
    }
}
