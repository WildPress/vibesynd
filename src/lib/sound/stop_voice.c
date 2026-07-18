/* stop_voice -- SKIP-ASM-IDIOM (voice-stop trampoline, 61B).
 * disasm: mov ebx,esp; mov ebx,[ebx+4]; if ((unsigned)ebx >= 0x10) ret;
 *   ebx <<= 2; edx=0; xchg [ebx+g_bd7a],edx;      ; atomically clear active flag
 *   if (old != 0) { e = [ebx+g_bd3a]; if (e != -1) FUN_000396d5(e); }
 *   mov eax,0x68; jmp sound_dispatch_trampoline                ; tail-dispatch key 0x68
 * The `mov ebx,esp` stack read, the `xchg` read-modify-write clear and the
 * computed tail `jmp` are hand asm; Watcom C emits none of them. Parked. */
extern void __b00039a82(void);
#pragma aux __b00039a82 = "db 139" "db 220" "db 139" "db 91" "db 4" "db 131" "db 251" "db 16" "db 115" "db 50" "db 209" "db 227" "db 209" "db 227" "db 186" "db 0" "db 0" "db 0" "db 0" "db 135" "db 147" "db 122" "db 189" "db 0" "db 0" "db 131" "db 250" "db 0" "db 116" "db 30" "db 139" "db 147" "db 58" "db 189" "db 0" "db 0" "db 131" "db 250" "db 255" "db 116" "db 9" "db 82" "db 232" "db 36" "db 252" "db 255" "db 255" "db 131" "db 196" "db 4" "db 184" "db 104" "db 0" "db 0" "db 0" "db 233" "db 238" "db 247" "db 255" "db 255" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux stop_voice modify [eax ebx ecx edx esi edi ebp];
void stop_voice(void) { __b00039a82(); }
