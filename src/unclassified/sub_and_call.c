/* frameless push-ebx @ 0x18488: g_3ef0 -= b; FUN_0003ab59(a); */
extern int g_3ef0;
extern void FUN_0003ab59(int a);
void sub_and_call(int a, int b)
{
    g_3ef0 -= b;
    FUN_0003ab59(a);
}
