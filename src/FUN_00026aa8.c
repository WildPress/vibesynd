/* frameless push-ebx @ 0x26aa8: return max(abs(a),abs(b)) with 16-bit compare */
extern int FUN_0003aed8(int x);
int FUN_00026aa8(short a, short b)
{
    int x = FUN_0003aed8(a);
    int y = FUN_0003aed8(b);
    if ((short)x > (short)y)
        return x;
    return y;
}
