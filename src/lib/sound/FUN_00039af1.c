/* dispatch stub @ 00039af1 (10B): mov eax,0x85; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int FUN_00039af1(void) { return sound_dispatch_trampoline(0x85); }
