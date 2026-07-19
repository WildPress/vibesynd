/* frameless push-ebx @ 0x18488: g_3ef0 -= b; free(a); */
extern int g_3ef0;
extern void free(int a);
void sub_and_call(int a, int b)
{
    g_3ef0 -= b;
    free(a);
}
