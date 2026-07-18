/* @ 0x392ac (18B) -- sound-driver dispatch trampoline. Reads the caller's first
   stack arg into EBX, calls the driver-table lookup driver_msg_lookup (returns a
   handler address in EAX), tail-jumps to it (JMP EAX) if non-null, else RET.
   All the 0x39xxx `mov eax,imm; jmp 0x392ac` stubs funnel through here.
   Hand-asm (caller-saves-all) -> db-transcribed; the trailing RET is Watcom's
   epilogue. */
extern int driver_msg_lookup(void);
extern void __t392ac(void);
#pragma aux __t392ac = "db 139" "db 220" "db 139" "db 91" "db 4" "call driver_msg_lookup" "db 131" "db 248" "db 0" "db 116" "db 2" "db 255" "db 224" modify exact [eax ebx ecx edx];
#pragma aux sound_dispatch_trampoline modify [eax ebx ecx edx];
void sound_dispatch_trampoline(void) { __t392ac(); }
