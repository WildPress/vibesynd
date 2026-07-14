/* dispatch stub @ 00039afb (10B): mov eax,0x7c; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int FUN_00039afb(void) { return sound_dispatch_trampoline(0x7c); }
