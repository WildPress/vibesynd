/* @ 0x00014c58 (26B): sum of squares then call.
 *   mov eax,[esp+4]; mov edx,[esp+8]; imul eax,eax; imul edx,edx; add eax,edx;
 *   push eax; call 0x4d393; add esp,4; ret   ->  return g(a*a + b*b) */
extern int isqrt32(int);
int sum_of_squares_call(int a, int b)
{
    return isqrt32(a * a + b * b);
}
