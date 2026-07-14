/* dispatch stub @ 00039bc3 (10B): mov eax,0xab; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int FUN_00039bc3(void) { return sound_dispatch_trampoline(0xab); }
