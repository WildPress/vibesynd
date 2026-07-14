/* dispatch stub @ 00039b05 (10B): mov eax,0x7d; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int FUN_00039b05(void) { return sound_dispatch_trampoline(0x7d); }
