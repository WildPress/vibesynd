/* frameless @ 0x37738: from p->[0x44] (id into object pool base g_entity_pool), if the
   node is live (signed word at +0x14 >= 0) store g_a6fe[node[0x19]] into p[0x46].

   NOTE: the field is loaded INLINE twice (no named `id`) on purpose. A named
   `unsigned short id` lets Watcom keep it in EAX and zero-extend in place
   (`and eax,0xffff`); the repeated inline subexpression makes Watcom CSE it into
   a callee-saved register (EBX) and zero-extend via `xor eax,eax; mov ax,bx` —
   which is what the target does (hence the push/pop ebx). Inlining a value's
   uses <-> forcing a persistent register. */
extern unsigned char g_entity_pool[];
extern unsigned char g_a6fe[];
void FUN_00037738(unsigned char *p)
{
    if (*(unsigned short *)(p + 0x44) != 0) {
        unsigned char *node = g_entity_pool + *(unsigned short *)(p + 0x44);
        if (*(short *)(node + 0x14) >= 0)
            p[0x46] = g_a6fe[node[0x19]];
    }
}
