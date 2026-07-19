/* dispatch stub @ 00039b5f (10B): mov eax,0x97; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int snd_cmd_97(void) { return sound_dispatch_trampoline(0x97); }
