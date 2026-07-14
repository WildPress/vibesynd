/* dispatch stub @ 000399b3 (10B): mov eax,0x65; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int FUN_000399b3(void) { return sound_dispatch_trampoline(0x65); }
