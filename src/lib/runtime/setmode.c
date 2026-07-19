/* GAME/CLIB hand-asm (0% RTL fingerprint, library region). setmode-style stream helper
   @ 0x3adf0. Frame PUSH EBX/ESI/EDI/EBP; MOV EBP,ESP. Arg0 (handle, [EBP+0x14]->EBX),
   arg1 (requested mode, [EBP+0x18]->ESI). Calls 0x3c529(handle) to fetch the current mode
   flags byte; if 0 sets errno(0x3c46d)=4 and returns -1. Derives the previous mode into EDI
   (0x200 when bit 0x40 set, else 0x100). If arg1 equals the current mode, returns it
   unchanged; if arg1 is 0x200 or 0x100 it rewrites the flag byte (clearing bit 0x40, setting
   it for 0x200) and pushes the fd back through 0x3c57b, then walks the open-file list at
   [0x11e4c] updating matching entries' flag byte at [+0xc]; an invalid arg1 sets errno=9 and
   returns -1. Returns the previous mode in EDI. Three CALL rel32s (0x3c529/0x3c46d/0x3c57b)
   are real extern calls (masked); the list-head address 0x11e4c is a literal db abs32. Body
   db-transcribed minus the trailing RET, split across three #pragma aux routines under the DOS
   wcc386 source-line limit (Watcom concatenates them contiguously so the internal E9/EB jumps
   resolve). Frameless void wrapper supplies the RET; all regs in the modify set. */
extern void fd_to_stream(void);
extern void errno_ptr(void);
extern void fd_handle_store(void);
extern void __adf0_a(void);
extern void __adf0_b(void);
extern void __adf0_c(void);
#pragma aux __adf0_a = "db 83" "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 93" "db 20" "db 139" "db 117" "db 24" "db 83" "call fd_to_stream" "db 137" "db 194" "db 131" "db 196" "db 4" "db 133" "db 192" "db 117" "db 21" "call errno_ptr" "db 199" "db 0" "db 4" "db 0" "db 0" "db 0" "db 184" "db 255" "db 255" "db 255" "db 255" "db 233" "db 129" "db 0" "db 0" "db 0" "db 168" "db 64" "db 116" "db 7" "db 186" "db 0" "db 2" "db 0" "db 0" "db 235" "db 5" "db 186" "db 0" "db 1" "db 0" "db 0" "db 137" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __adf0_b = "db 215" "db 57" "db 214" "db 15" "db 132" "db 101" "db 0" "db 0" "db 0" "db 129" "db 254" "db 0" "db 2" "db 0" "db 0" "db 116" "db 8" "db 129" "db 254" "db 0" "db 1" "db 0" "db 0" "db 117" "db 69" "db 36" "db 191" "db 129" "db 254" "db 0" "db 2" "db 0" "db 0" "db 117" "db 2" "db 12" "db 64" "db 80" "db 83" "call fd_handle_store" "db 161" "db 76" "db 30" "db 1" "db 0" "db 131" "db 196" "db 8" "db 235" "db 34" "db 139" "db 80" "db 4" "db 131" "db 122" "db 12" "db 0" "db 116" "db 23" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux __adf0_c = "db 59" "db 90" "db 16" "db 117" "db 18" "db 128" "db 98" "db 12" "db 191" "db 129" "db 254" "db 0" "db 2" "db 0" "db 0" "db 117" "db 30" "db 128" "db 74" "db 12" "db 64" "db 235" "db 24" "db 139" "db 0" "db 133" "db 192" "db 117" "db 218" "db 235" "db 16" "call errno_ptr" "db 199" "db 0" "db 9" "db 0" "db 0" "db 0" "db 233" "db 119" "db 255" "db 255" "db 255" "db 137" "db 248" "db 93" "db 95" "db 94" "db 91" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux setmode modify [eax ebx ecx edx esi edi ebp];
void setmode(void)
{
    __adf0_a();
    __adf0_b();
    __adf0_c();
}
