/* @ 0x3c479 -- framed CLIB helper: if(b) return dosret_map_err(a,b); else b. */
extern int dosret_map_err(int a, int b);
int dosret(int a, int b)
{
    if (b != 0)
        return dosret_map_err(a, b);
    return b;
}
