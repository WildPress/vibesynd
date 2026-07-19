/* @ 0x3c491 -- framed CLIB helper: if(flag) p = set_doserr(p); return p. */
extern int set_doserr(int p);
int dosret_map_err(int p, int flag)
{
    if (flag != 0)
        p = set_doserr(p);
    return p;
}
