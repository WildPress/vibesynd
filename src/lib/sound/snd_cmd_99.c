/* dispatch stub @ 00039b73 (10B): mov eax,0x99; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int snd_cmd_99(void) { return sound_dispatch_trampoline(0x99); }
