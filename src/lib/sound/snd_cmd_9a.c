/* dispatch stub @ 00039b7d (10B): mov eax,0x9a; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int snd_cmd_9a(void) { return sound_dispatch_trampoline(0x9a); }
