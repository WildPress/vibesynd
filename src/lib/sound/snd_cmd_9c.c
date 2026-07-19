/* dispatch stub @ 00039b91 (10B): mov eax,0x9c; jmp sound_dispatch_trampoline (tail call) */
extern int sound_dispatch_trampoline(int);
int snd_cmd_9c(void) { return sound_dispatch_trampoline(0x9c); }
