/* PARKED NEAR-MISS (ours 307B vs target 313B, EDIT-DIST 45, down from 59).
   TAIL NOW BYTE-EXACT: the closing status-read + word+4 clear + neg reproduce
   the target instruction-for-instruction (lgs ECX, xor eax, mov al gs:[ecx+31],
   mov word gs:[ecx+4],0, neg eax). Two fixes closed it: (1) SHARE one far ptr
   `p = sel :> (uchar*)off` for both the [0x31] read and the +4 clear (target
   emits a single lgs; the old two-`sel:>off` spelling forced a second
   mov ecx,[esp+0x18] re-read), (2) `int st` not `unsigned char st` so the
   status widens xor-first (`xor eax,eax; mov al`) instead of the and-form
   (`mov al; and eax,0xff`) the named byte local produced. Both are faithful
   reconstructions, not byte-tricks.
   Remaining gap = the 0x27fc8/0x28118 EAX<->ECX register-role tie-break wall,
   one cascade seeded at the 0x91-stamp lgs: target lgs ECX + fresh
   `mov ebp,[esp+0x18]; add ebp,0xa; mov eax,ebp` vs ours lgs EAX +
   `add eax,0xa; mov ebp,eax`; then mid-block byte temps AL vs CL/DL, seg widen
   xor ecx/mov cx,gs vs xor eax, `movsx ecx,ax` vs cwde after 0x27d88 (2B),
   busy-wait AH vs DL, report offset in EBP vs ECX. Levers tried (all compile,
   all park at the entry state): (off,sel) split vs 8-byte far-ptr param
   (byte-identical); statement q vs assignment-in-arg q vs named ushort/
   `__segment` sel local vs named far nm; noff local computed before/after the
   stamp (noff-first splits the loads but mirrors roles: lgs EBP + fresh EAX);
   q decl+assign before stamp (50); stamp-after-fstrcpy (59); component near-
   offset + segment local (canonicalizes); 0x28118's pragma modify lists;
   -or (worse, 56); -of/-4r (103). Definitive wall evidence: 0x28558's MATCHED
   target emits cwde from the same `if (submit_ncb(...) == -1)` spelling that
   emits movsx ecx,ax here -- allocator state, not source-reachable.
   Recipe: -4s -oneatx -zp8 -s -zq

   netbios_op91 @ 0x28228: NetBIOS session-op (opcode 0x91), same family as
   parked 0x27fc8/0x28118 and matched 0x284a8/0x28558. Stamps NCB[0] = 0x91, copies
   `name` into the name field at +0xa via an inlined far strcpy
   (db-transcribed pragma), pads with g_name_pad (far strcat) until far strlen
   >= 15, sets timeout bytes +0x2a/+0x2b from the two byte params, submits
   via FUN_27d88 (-1 -> -0x63). async != 0 -> return 0 (duplicated
   epilogue). Busy-wait status [0x31] != 0xff (unnamed inline reads, AH),
   report nonzero status via 0x289a8(g_376c, 0x217, st), then clear word +4
   and return -status. Pragma modify lists follow 0x28118's proven set. */
extern char g_name_pad[];
extern char g_376c[];
extern short submit_ncb(unsigned char __far *p);
extern void report_net_status(char *s, int b, int c);

extern void fstrcpy91(unsigned char __far *dst, unsigned char __far *src);
#pragma aux fstrcpy91 = "db 30" "db 6" "db 80" "db 102" "db 142" "db 194" "db 102" "db 142" "db 217" "db 139" "db 243" "db 139" "db 248" "db 138" "db 6" "db 38" "db 136" "db 7" "db 60" "db 0" "db 15" "db 132" "db 17" "db 0" "db 0" "db 0" "db 138" "db 70" "db 1" "db 131" "db 198" "db 2" "db 38" "db 136" "db 71" "db 1" "db 131" "db 199" "db 2" "db 60" "db 0" "db 117" "db 226" "db 88" "db 7" "db 31" parm [dx eax] [cx ebx] modify [esi edi];

extern unsigned fstrlen91(unsigned char __far *s);
#pragma aux fstrlen91 = "db 6" "db 142" "db 193" "db 49" "db 192" "db 137" "db 193" "db 73" "db 242" "db 174" "db 247" "db 209" "db 73" "db 7" parm [cx edi] value [ecx] modify [eax];

extern void fstrcat91(unsigned char __far *dst, unsigned char __far *src);
#pragma aux fstrcat91 = "db 30" "db 6" "db 80" "db 102" "db 142" "db 194" "db 102" "db 142" "db 217" "db 139" "db 243" "db 139" "db 248" "db 43" "db 201" "db 73" "db 176" "db 0" "db 242" "db 174" "db 79" "db 138" "db 6" "db 170" "db 60" "db 0" "db 15" "db 132" "db 11" "db 0" "db 0" "db 0" "db 138" "db 70" "db 1" "db 131" "db 198" "db 2" "db 170" "db 60" "db 0" "db 117" "db 234" "db 88" "db 7" "db 31" parm [dx eax] [cx ebx] modify exact [eax ecx esi edi];

int netbios_op91(unsigned int off, unsigned short sel, char *name,
                 unsigned short async, char rto, char sto)
{
    unsigned char __far *q;

    (sel :> (unsigned char *)off)[0] = 0x91;
    fstrcpy91(q = sel :> ((unsigned char *)off + 0xa), (unsigned char __far *)name);
    while (fstrlen91(q) < 0xf)
        fstrcat91(q, (unsigned char __far *)g_name_pad);
    (sel :> (unsigned char *)off)[0x2a] = rto;
    (sel :> (unsigned char *)off)[0x2b] = sto;
    if (submit_ncb(sel :> (unsigned char *)off) == -1)
        return -0x63;
    if (async != 0)
        return 0;
    while ((sel :> (unsigned char *)off)[0x31] == 0xff)
        ;
    if ((sel :> (unsigned char *)off)[0x31] != 0)
        report_net_status(g_376c, 0x217, (sel :> (unsigned char *)off)[0x31]);
    {
        unsigned char __far *p = sel :> (unsigned char *)off;
        int st = p[0x31];
        *(unsigned short __far *)(p + 4) = 0;
        return -st;
    }
}
