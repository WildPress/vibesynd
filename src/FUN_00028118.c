/* PARKED NEAR-MISS (256B vs 258B target; NOT matched, but cluster A's sel
 * half is now SOLVED via the split-param + assignment-in-arg lever from
 * matched 0x28558/0x284a8).
 * Current state (retry cont.20, 6 compiles): signature split into
 * (off, sel, src) dwords (ABI-identical to the old far-ptr param) and
 * `fstrcpy118(q = sel :> ((uchar*)off+0x1a), src)` assignment-IN-ARG gives
 * the target's parm-driven EDX sel staging + pre-block spill from EDX
 * (mov edx,[esp+0x1c] ... mov [esp],edx) and the 22B entry length. Remaining
 * entry diff is ONLY the off half role swap: ours computes off+0x1a in parm
 * EAX then `mov ebp,eax`; target computes in home EBP then `mov eax,ebp`
 * (8b44/83c0/89c5 vs 8b6c/83c5/89e8, plus mov cx,ds scheduling). Tried and
 * rejected: named `o = off+0x1a` before an in-arg q (collapses back to EAX
 * compute), statement `q = sel :> o` (reverts to ECX staging + EDX param
 * re-read, 260B), pure inline no-q (target entry shape but NO spill slot at
 * all -> param-slot re-reads in the pad loop + duplicated epilogues, 252B),
 * ushort-typed seg local (zero-extend xor/word-mov, 259B), named middle p
 * single or per-arm reassigned (Watcom caches the selector in SI/DX between
 * arms, 259-272B).
 * Cluster B (middle, ~12B spread) unchanged: one allocator tie-break at the
 * 0xb1-stamp lgs (EAX ours vs ECX target) propagates down the whole arm
 * chain: seg widen xor/mov cx,gs reg, cwde (ours, -2B) vs movsx ecx,ax,
 * cmp reg, wait lgs base + AH-vs-DL loop temp. Our own FINAL arm already
 * picks ECX (EAX reserved by the return widen). short-return prototype vs
 * (short) cast on int return both give cwde. Fuzzer/permute may close B;
 * entry off-half likely needs the same treatment.
 * -oneatx and -or give byte-identical output for this fn.
 *
 * FUN_00028118 @ 0x28118 - DOS/DPMI transfer-buffer request, opcode 0xb1
 * (open file by name?). Takes a far ptr to the request block (8-byte stack
 * far-ptr param) and a near filename string. Copies the name into block+0x1a
 * via an inlined far strcpy (2-byte-unrolled, push ds/es wrapper), then pads
 * with the string at 0x377c (far strcat with repne-scasb seek) until the far
 * strlen is >= 0xf. Stamps byte[0] = 0xb1, submits the far ptr via 0x27d88
 * (returns short; -1 => -0x63 through the shared epilogue), busy-waits on
 * status byte [0x31] until != 0xff, reports failure via 0x3ad66(g_37b8, src)
 * and returns -status as int.
 * The three far-string helpers are db-transcribed #pragma aux bodies
 * (Watcom string.h far-string inline pragmas: 66-prefixed mov es/ds, 8b-form
 * reg moves, long-form forward je). fstrcpy clobbers its parm regs (forces
 * the q.seg spill to [esp] and reload); fstrcat preserves DX/EBX so its parm
 * loads hoist out of the pad loop; fstrlen preserves EDX/EBX.
 * Busy-wait/status reads are UNNAMED inline derefs (loop temp in AH, fresh
 * xor/mov al reads) as proven on 0x284a8. lgs re-materialisations of the
 * far-ptr param are automatic.
 * Recipe: -4s -or -zp8 -s -zq
 */
extern char g_377c[];
extern char g_37b8[];
extern short FUN_00027d88(unsigned char __far *p);
extern void FUN_0003ad66(char *fmt, char *s);

extern void fstrcpy118(unsigned char __far *dst, unsigned char __far *src);
#pragma aux fstrcpy118 = "db 30" "db 6" "db 80" "db 102" "db 142" "db 194" "db 102" "db 142" "db 217" "db 139" "db 243" "db 139" "db 248" "db 138" "db 6" "db 38" "db 136" "db 7" "db 60" "db 0" "db 15" "db 132" "db 17" "db 0" "db 0" "db 0" "db 138" "db 70" "db 1" "db 131" "db 198" "db 2" "db 38" "db 136" "db 71" "db 1" "db 131" "db 199" "db 2" "db 60" "db 0" "db 117" "db 226" "db 88" "db 7" "db 31" parm [dx eax] [cx ebx] modify [esi edi];

extern unsigned fstrlen118(unsigned char __far *s);
#pragma aux fstrlen118 = "db 6" "db 142" "db 193" "db 49" "db 192" "db 137" "db 193" "db 73" "db 242" "db 174" "db 247" "db 209" "db 73" "db 7" parm [cx edi] value [ecx] modify [eax];

extern void fstrcat118(unsigned char __far *dst, unsigned char __far *src);
#pragma aux fstrcat118 = "db 30" "db 6" "db 80" "db 102" "db 142" "db 194" "db 102" "db 142" "db 217" "db 139" "db 243" "db 139" "db 248" "db 43" "db 201" "db 73" "db 176" "db 0" "db 242" "db 174" "db 79" "db 138" "db 6" "db 170" "db 60" "db 0" "db 15" "db 132" "db 11" "db 0" "db 0" "db 0" "db 138" "db 70" "db 1" "db 131" "db 198" "db 2" "db 170" "db 60" "db 0" "db 117" "db 234" "db 88" "db 7" "db 31" parm [dx eax] [cx ebx] modify exact [eax ecx esi edi];

int FUN_00028118(unsigned int off, unsigned short sel, char *src)
{
    unsigned char __far *q;

    fstrcpy118(q = sel :> ((unsigned char *)off + 0x1a), (unsigned char __far *)src);
    while (fstrlen118(q) < 0xf)
        fstrcat118(q, (unsigned char __far *)g_377c);
    (sel :> (unsigned char *)off)[0] = 0xb1;
    if (FUN_00027d88(sel :> (unsigned char *)off) == -1)
        return -0x63;
    while ((sel :> (unsigned char *)off)[0x31] == 0xff)
        ;
    if ((sel :> (unsigned char *)off)[0x31] != 0)
        FUN_0003ad66(g_37b8, src);
    return -(int)(sel :> (unsigned char *)off)[0x31];
}
