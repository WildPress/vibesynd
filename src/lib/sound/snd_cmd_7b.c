/* dispatch stub @ 00039ae7 (10B): mov eax,0x7b; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int snd_cmd_7b(void) { return sound_dispatch_trampoline(0x7b); }
