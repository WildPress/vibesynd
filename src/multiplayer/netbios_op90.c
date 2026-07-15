/* PARKED NEAR-MISS (ours 305B vs target 311B, structure exact) -- same
   EAX<->ECX register-role tie-break wall as its twin 0x28228 (see that
   file's header for the full lever list; identical failure signature).
   The FINAL arm here already matches byte-exact (lgs ECX + xor eax,eax +
   mov al + neg -- EAX reserved by the return widen flips the allocator to
   the target roles, proving the mechanism); the top/mid/busy-wait/report
   arms stay EAX-seeded in ours vs ECX in target, plus cwde vs movsx (2B)
   and the top lea vs reload+add (4B). Fuzzer/cpermute may close.
   Recipe: -4s -oneatx -zp8 -s -zq

   netbios_op90 @ 0x28368: NetBIOS session-op (opcode 0x90), twin of 0x28228 (0x91) in the
   0x27fc8/0x28118/0x284a8/0x28558 family. Stamps NCB[0] = 0x90, copies `name`
   into the name field at +0xa via an inlined far strcpy (db-transcribed
   pragma), pads with g_name_pad (far strcat) until far strlen >= 15, sets timeout
   bytes +0x2a/+0x2b, submits via FUN_27d88 (-1 -> -0x63). async != 0 ->
   return 0 (duplicated epilogue). Busy-wait status [0x31] != 0xff (unnamed
   inline reads, AH); report via 0x289a8(g_376c, 0x234, st) unless status is
   0 or 0x14 (0x14 = NetBIOS "no answer"?); return -status. Unlike 0x28228
   there is no word+4 clear in the tail. Pragma modify lists follow
   0x28118's proven set. */
extern char g_name_pad[];
extern char g_376c[];
extern short submit_ncb(unsigned char __far *p);
extern void FUN_000289a8(char *s, int b, int c);

extern void fstrcpy90(unsigned char __far *dst, unsigned char __far *src);
#pragma aux fstrcpy90 = "db 30" "db 6" "db 80" "db 102" "db 142" "db 194" "db 102" "db 142" "db 217" "db 139" "db 243" "db 139" "db 248" "db 138" "db 6" "db 38" "db 136" "db 7" "db 60" "db 0" "db 15" "db 132" "db 17" "db 0" "db 0" "db 0" "db 138" "db 70" "db 1" "db 131" "db 198" "db 2" "db 38" "db 136" "db 71" "db 1" "db 131" "db 199" "db 2" "db 60" "db 0" "db 117" "db 226" "db 88" "db 7" "db 31" parm [dx eax] [cx ebx] modify [esi edi];

extern unsigned fstrlen90(unsigned char __far *s);
#pragma aux fstrlen90 = "db 6" "db 142" "db 193" "db 49" "db 192" "db 137" "db 193" "db 73" "db 242" "db 174" "db 247" "db 209" "db 73" "db 7" parm [cx edi] value [ecx] modify [eax];

extern void fstrcat90(unsigned char __far *dst, unsigned char __far *src);
#pragma aux fstrcat90 = "db 30" "db 6" "db 80" "db 102" "db 142" "db 194" "db 102" "db 142" "db 217" "db 139" "db 243" "db 139" "db 248" "db 43" "db 201" "db 73" "db 176" "db 0" "db 242" "db 174" "db 79" "db 138" "db 6" "db 170" "db 60" "db 0" "db 15" "db 132" "db 11" "db 0" "db 0" "db 0" "db 138" "db 70" "db 1" "db 131" "db 198" "db 2" "db 170" "db 60" "db 0" "db 117" "db 234" "db 88" "db 7" "db 31" parm [dx eax] [cx ebx] modify exact [eax ecx esi edi];

int netbios_op90(unsigned int off, unsigned short sel, char *name,
                 unsigned short async, char rto, char sto)
{
    unsigned char __far *q;

    (sel :> (unsigned char *)off)[0] = 0x90;
    fstrcpy90(q = sel :> ((unsigned char *)off + 0xa), (unsigned char __far *)name);
    while (fstrlen90(q) < 0xf)
        fstrcat90(q, (unsigned char __far *)g_name_pad);
    (sel :> (unsigned char *)off)[0x2a] = rto;
    (sel :> (unsigned char *)off)[0x2b] = sto;
    if (submit_ncb(sel :> (unsigned char *)off) == -1)
        return -0x63;
    if (async != 0)
        return 0;
    while ((sel :> (unsigned char *)off)[0x31] == 0xff)
        ;
    if ((sel :> (unsigned char *)off)[0x31] != 0 &&
        (sel :> (unsigned char *)off)[0x31] != 0x14)
        FUN_000289a8(g_376c, 0x234, (sel :> (unsigned char *)off)[0x31]);
    return -(int)(sel :> (unsigned char *)off)[0x31];
}
