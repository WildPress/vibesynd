/* @ 0x000188c8 (26B): forward three args to a callee (same shape as file_read_n).
   The callee is write @0x3ab8e (original 0x188d8: `call 0x3ab8e`); the prior
   FUN_0003a7e0 symbol was a mis-transcribed target (0x3a7e0 lands mid-`read`). */
extern void write(int, int, int);
void file_write(int a, int b, int c)
{
    write(a, b, c);
}
