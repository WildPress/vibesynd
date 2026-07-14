/* dispatch stub @ 00039bb9 (10B): mov eax,0xaa; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int FUN_00039bb9(void) { return sound_dispatch_trampoline(0xaa); }
