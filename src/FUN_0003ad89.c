/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Non-returning exit trampoline @ 0x3ad89.
   Runs two chained indirect calls through the global function-pointer slots at [0xbff4] and
   [0xbff8] (atexit/flush handlers), then re-pushes its return address ([EBP+4]) and re-enters
   the same style of frame to call [0xbff8] and [0xbffc] again, finally tail-calling 0x3b1d1
   (JMP _exit) with the caller return address in EAX -- so control never comes back. The
   indirect calls' absolute pointer addresses (0xbff4/0xbff8/0xbffc) are literal FF15 db bytes
   (no fixup on our side). The final CALL 0x3b1d1 is a real extern call; match_reloc masks the
   reloc. Whole body db-transcribed INCLUDING the terminating call (there is no RET); the
   wrapper is marked `aborts` so Watcom emits no trailing return. All regs in the modify set. */
extern void FUN_0003b1d1(void);
#pragma aux FUN_0003b1d1 aborts;
extern void __ad89body(void);
#pragma aux __ad89body = "db 85" "db 137" "db 229" "db 255" "db 21" "db 244" "db 191" "db 0" "db 0" "db 255" "db 21" "db 248" "db 191" "db 0" "db 0" "db 255" "db 117" "db 4" "db 85" "db 137" "db 229" "db 255" "db 21" "db 248" "db 191" "db 0" "db 0" "db 255" "db 21" "db 252" "db 191" "db 0" "db 0" "db 139" "db 69" "db 4" "call FUN_0003b1d1" aborts modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux FUN_0003ad89 aborts modify [eax ebx ecx edx esi edi ebp];
void FUN_0003ad89(void)
{
    __ad89body();
}
