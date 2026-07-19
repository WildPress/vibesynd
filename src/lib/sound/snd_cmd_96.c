/* dispatch stub @ 00039b55 (10B): mov eax,0x96; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int snd_cmd_96(void) { return sound_dispatch_trampoline(0x96); }
