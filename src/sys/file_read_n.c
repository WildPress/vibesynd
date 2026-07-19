/* @ 0x000188a8 (26B): forward three args to a callee (g uses ebx as scratch). */
extern void read(int, int, int);
void file_read_n(int a, int b, int c)
{
    read(a, b, c);
}
