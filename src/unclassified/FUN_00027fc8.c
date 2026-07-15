/* PARKED near-miss (NOT matched, ours 324B vs target 326B, byte-identical through 0xa0,
   raw 182/326 after the 2-byte shift) -- register-role tie-break wall. Structure is exact:
   inline _fstrcpy/_fstrlen/_fstrcat (they DO inline via #pragma intrinsic, like _fmemset),
   the pad loop, both calls, busy-wait, and the middle section's `mov gs,[esp+0x1c]` word
   reloads + offset cached in a callee-saved reg (won via component locals o/sel below, with
   sel = (__segment)p REASSIGNED after each call so it re-reads the param slot instead of a
   BX spill -- new lever). Remaining diffs, all EAX<->ECX scratch tie-breaks our 9.5b won't
   flip: lgs ECX vs EAX at the 0xb0 store / busy-wait / final return; xor ecx widens vs
   xor eax; movsx ecx,ax vs cwde after the 0x27d88 call (the 2-byte size delta); dl vs dh
   at the if-load; o init `mov ebp,[esp+0x18]` vs our CSE copy `mov ebp,eax`; site-1 pair
   copy `mov ecx,ebp`. 10 spellings tried (named q vs inline p+0x1a, short/int r, far local
   m everywhere, (__segment) inline vs named, &p alias reads, decl-order swaps).
   RETRY (inline-construction lever from matched 0x28558) also failed, +5 spellings:
   (1) split (off,sel) params w/ sel :> (unsigned char *)off at every site -- WRONG
   SHAPE for this fn: target does `lgs ecx,[esp+0x18]`, i.e. a genuine 8-byte far-ptr
   param rematerialized whole, and the split loses the `sub esp,4` sel spill slot
   (311B, diff@0x4). (2) drop the volatile on the busy-wait, (3) short r (hoping
   movsx ecx,ax), (4) inline offset `(sel :> (unsigned char *)p)[0x31]` at all 3
   middle sites -- ALL THREE compile BYTE-IDENTICAL to this parked 324B output
   (Watcom canonicalizes them); `((__segment)p :> ...)` inline is a syntax error
   (:> wants a named segment lvalue). (5) -or recipe restructures the whole fn
   (lea-based, no spill slot, 314B) -- -oneatx confirmed. Target evidence recap:
   site-1 arg staging is `mov ebx,[esp+0x20]; mov ebp,[esp+0x18]` INSIDE the arg
   eval (no CSE from the busy-wait lgs), every scratch in 0xa1..0x140 is ECX/EDX
   with EAX only at -99 and the final widen; ours mirrors the shape exactly one
   register off (EAX-first). Genuine allocator-state wall; fuzzer/cpermute may close.
   Recipe: -4s -oneatx -zp8 -s -zq

   FUN_00027fc8 @ 0x27fc8 - submit a named command through the real-mode mailbox block.
   Copy `name` into the far block at p+0x1a (inline _fstrcpy), pad it with g_name_pad
   (inline _fstrcat) until _fstrlen >= 15, stamp command byte p[0]=0xb0, submit via
   0x27d88 (returns -99 if that fails), busy-wait while the status byte p[0x31] stays
   0xff, log via printf 0x3ad66 (g_3780, status, name); if status not 0/0x16 also log
   g_3798 and report 0x289a8(g_376c, 0x1e6, status). Returns -status. */

extern short FUN_00027d88(unsigned char __far *p);
extern void FUN_0003ad66(char *fmt, ...);
extern void FUN_000289a8(char *s, int line, int code);

extern char g_name_pad[];   /* pad chunk appended until name field is 15 chars */
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
    __segment sel;
    int r;

    _fstrcpy((char __far *)p + 0x1a, name);
    while (_fstrlen((char __far *)p + 0x1a) < 0xf)
        _fstrcat((char __far *)p + 0x1a, g_name_pad);

    *p = 0xb0;
    r = FUN_00027d88(p);
    if (r == -1)
        return -99;
    while (*(volatile unsigned char __far *)(p + 0x31) == 0xff)
        ;
    o = (unsigned char *)p;
    sel = (__segment)p;
    FUN_0003ad66(g_3780, (sel :> o)[0x31], name);
    sel = (__segment)p;
    if ((sel :> o)[0x31] != 0 && (sel :> o)[0x31] != 0x16) {
        FUN_0003ad66(g_3798, name);
        sel = (__segment)p;
        FUN_000289a8(g_376c, 0x1e6, (sel :> o)[0x31]);
    }
    return -(int)p[0x31];
}
