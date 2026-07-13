/* FUN_00027fc8 @ 0x27fc8 - submit a named command through the real-mode mailbox block.
   Copy `name` into the far block at p+0x1a (inline _fstrcpy), pad it with g_377c
   (inline _fstrcat) until _fstrlen >= 15, stamp command byte p[0]=0xb0, submit via
   0x27d88 (returns -99 if that fails), busy-wait while the status byte p[0x31] stays
   0xff, log via printf 0x3ad66 (g_3780, status, name); if status not 0/0x16 also log
   g_3798 and report 0x289a8(g_376c, 0x1e6, status). Returns -status. */

extern short FUN_00027d88(unsigned char __far *p);
extern void FUN_0003ad66(char *fmt, ...);
extern void FUN_000289a8(char *s, int line, int code);

extern char g_377c[];   /* pad chunk appended until name field is 15 chars */
extern char g_3780[];   /* "loaded ..." style format */
extern char g_3798[];
extern char g_376c[];

extern char __far *_fstrcpy(char __far *dst, const char __far *src);
extern unsigned _fstrlen(const char __far *s);
extern char __far *_fstrcat(char __far *dst, const char __far *src);
#pragma intrinsic(_fstrcpy)
#pragma intrinsic(_fstrlen)
#pragma intrinsic(_fstrcat)

int FUN_00027fc8(unsigned char __far *p, char *name)
{
    unsigned char *o;
    int r;

    _fstrcpy((char __far *)p + 0x1a, name);
    while (_fstrlen((char __far *)p + 0x1a) < 0xf)
        _fstrcat((char __far *)p + 0x1a, g_377c);

    *p = 0xb0;
    r = FUN_00027d88(p);
    if (r == -1)
        return -99;
    while (*(volatile unsigned char __far *)(p + 0x31) == 0xff)
        ;
    o = (unsigned char *)p;
    FUN_0003ad66(g_3780, ((__segment)p :> o)[0x31], name);
    if (((__segment)p :> o)[0x31] != 0 && ((__segment)p :> o)[0x31] != 0x16) {
        FUN_0003ad66(g_3798, name);
        FUN_000289a8(g_376c, 0x1e6, ((__segment)p :> o)[0x31]);
    }
    return -(int)p[0x31];
}
