/* frameless push-ebx @ 0x26aa8: return max(abs(a),abs(b)) with 16-bit compare */
extern int labs(int x);
int max_abs(short a, short b)
{
    int x = labs(a);
    int y = labs(b);
    if ((short)x > (short)y)
        return x;
    return y;
}
