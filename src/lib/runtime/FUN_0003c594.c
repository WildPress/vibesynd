/* C runtime: near-heap allocator core @ 0x3c594 (CLIB3S, Watcom 9.5). Register-calling
   hand-asm: EAX=requested size, DX=arena segment selector (loaded into DS), EBX=arena
   descriptor. Rounds the size up to a 4-byte multiple + header, walks the free-list of
   the arena (ptr fields at [ebx+8]/[ebx+0xc]/[ebx+0x10]/[ebx+0x1c]/[ebx+0x24]), finds a
   block >= size, splits it (or unlinks it whole), marks it in-use (OR [esi],1) and
   returns the payload pointer ESI+4; returns 0 on failure. Frameless: the body saves
   esi/edi/ecx/ds itself (push...pop), so a parameterless wrapper + modify set emits the
   body with no param-load prologue and no compiler save. Body db-transcribed minus the
   trailing RET (the wrapper supplies it). No external calls, no fixups (all refs
   register-relative). Split across two #pragma aux (DOS line limit; Watcom concatenates
   contiguously so the internal jumps resolve). */
extern void __nheap_a(void);
extern void __nheap_b(void);
#pragma aux __nheap_a = "db 86" "db 87" "db 81" "db 30" "db 142" "db 218" "db 11" "db 192" "db 116" "db 75" "db 5" "db 7" "db 0" "db 0" "db 0" "db 114" "db 68" "db 36" "db 252" "db 186" "db 12" "db 0" "db 0" "db 0" "db 43" "db 194" "db 245" "db 27" "db 201" "db 35" "db 193" "db 3" "db 194" "db 59" "db 67" "db 16" "db 119" "db 47" "db 139" "db 115" "db 8" "db 139" "db 123" "db 12" "db 59" "db 199" "db 119" "db 5" "db 139" "db 115" "db 36" "db 43" "db 255" "db 139" "db 22" "db 59" "db 194" "db 15" "db 134" "db 26" "db 0" "db 0" "db 0" "db 43" "db 250" "db 245" "db 27" "db 201" "db 35" "db 249" "db 3" "db 250" "db 141" "db 83" "db 28" "db 139" "db 118" "db 8" "db 59" "db 242" "db 117" "db 227" "db 137" "db 123" "db 16" "db 43" "db 192" "db 235" modify exact [eax ebx ecx edx esi edi];
#pragma aux __nheap_b = "db 77" "db 43" "db 208" "db 137" "db 123" "db 12" "db 255" "db 67" "db 20" "db 139" "db 78" "db 8" "db 131" "db 250" "db 12" "db 114" "db 30" "db 139" "db 254" "db 3" "db 248" "db 137" "db 123" "db 8" "db 137" "db 23" "db 137" "db 6" "db 139" "db 94" "db 4" "db 137" "db 95" "db 4" "db 137" "db 79" "db 8" "db 137" "db 123" "db 8" "db 139" "db 217" "db 137" "db 123" "db 4" "db 235" "db 21" "db 255" "db 75" "db 24" "db 139" "db 198" "db 139" "db 249" "db 139" "db 118" "db 4" "db 137" "db 126" "db 8" "db 137" "db 119" "db 4" "db 137" "db 115" "db 8" "db 139" "db 240" "db 131" "db 14" "db 1" "db 139" "db 198" "db 5" "db 4" "db 0" "db 0" "db 0" "db 31" "db 89" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003c594 modify [eax ebx ecx edx esi edi];
void FUN_0003c594(void)
{
    __nheap_a();
    __nheap_b();
}
