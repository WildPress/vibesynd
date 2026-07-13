/* PARKED NEAR-MISS (258/258 length, structure fully byte-correct, two
 * register-role clusters remain; NOT matched).
 * Cluster A (entry, ~14B): ours stages q's segment dword through EAX
 * (mov eax,[esp+0x1c]; mov [esp],eax; mov edx,eax) where the target loads
 * EDX directly and spills from it (mov edx,[esp+0x1c]; ...; mov [esp],edx).
 * With ALL-INLINE `p + 0x1a` args (no q variable) the entry matches through
 * 0x19 and the load IS parm-driven EDX, but Watcom then re-reads the param
 * slot AFTER the strcpy block for the loop's CSE temp instead of saving EDX
 * pre-block (260B). Named q => assignment-driven EAX staging; inline =>
 * parm-driven EDX but post-block re-read. The target needs both parm-driven
 * EDX AND pre-block spill; not source-reachable in 11 spellings (q=p+0x1a,
 * q=p;q+=0x1a, assignment-in-arg, poff/segd split locals, inline-first).
 * Cluster B (middle, ~10B): pure EAX<->ECX scratch swap: target lgs ECX for
 * the 0xb1 stamp, xor ecx/mov cx,gs seg widen, movsx ecx,ax result, AH wait
 * loop on [ecx+0x31]; ours picks EAX (cwde, DL loop temp on [eax+0x31]).
 * Our own FINAL section picks ECX (EAX reserved by the return widen), so the
 * pick is context-driven allocator tie-break (playbook #3 register-role wall).
 * -oneatx and -or give byte-identical output for this fn. Fuzzer may close.
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
extern int FUN_00027d88(unsigned char __far *p);
extern void FUN_0003ad66(char *fmt, char *s);

extern void fstrcpy118(unsigned char __far *dst, unsigned char __far *src);
#pragma aux fstrcpy118 = "db 30" "db 6" "db 80" "db 102" "db 142" "db 194" "db 102" "db 142" "db 217" "db 139" "db 243" "db 139" "db 248" "db 138" "db 6" "db 38" "db 136" "db 7" "db 60" "db 0" "db 15" "db 132" "db 17" "db 0" "db 0" "db 0" "db 138" "db 70" "db 1" "db 131" "db 198" "db 2" "db 38" "db 136" "db 71" "db 1" "db 131" "db 199" "db 2" "db 60" "db 0" "db 117" "db 226" "db 88" "db 7" "db 31" parm [dx eax] [cx ebx] modify [esi edi];

extern unsigned fstrlen118(unsigned char __far *s);
#pragma aux fstrlen118 = "db 6" "db 142" "db 193" "db 49" "db 192" "db 137" "db 193" "db 73" "db 242" "db 174" "db 247" "db 209" "db 73" "db 7" parm [cx edi] value [ecx] modify [eax];

extern void fstrcat118(unsigned char __far *dst, unsigned char __far *src);
#pragma aux fstrcat118 = "db 30" "db 6" "db 80" "db 102" "db 142" "db 194" "db 102" "db 142" "db 217" "db 139" "db 243" "db 139" "db 248" "db 43" "db 201" "db 73" "db 176" "db 0" "db 242" "db 174" "db 79" "db 138" "db 6" "db 170" "db 60" "db 0" "db 15" "db 132" "db 11" "db 0" "db 0" "db 0" "db 138" "db 70" "db 1" "db 131" "db 198" "db 2" "db 170" "db 60" "db 0" "db 117" "db 234" "db 88" "db 7" "db 31" parm [dx eax] [cx ebx] modify exact [eax ecx esi edi];

int FUN_00028118(unsigned char __far *p, char *src)
{
    unsigned char __far *q;

    q = p + 0x1a;
    fstrcpy118(q, (unsigned char __far *)src);
    while (fstrlen118(q) < 0xf)
        fstrcat118(q, (unsigned char __far *)g_377c);
    p[0] = 0xb1;
    if ((short)FUN_00027d88(p) == -1)
        return -0x63;
    while (p[0x31] == 0xff)
        ;
    if (p[0x31] != 0)
        FUN_0003ad66(g_37b8, src);
    return -(int)p[0x31];
}
