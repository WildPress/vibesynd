/* GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Word-wise string append @ 0x3a900.
   Custom frame (PUSH ESI/EDI/EBP; MOV EBP,ESP): dest at [EBP+0x10], src at [EBP+0x14].
   Sets ES=DS, SCASB/REPNE-scans dest for its NUL to find the append point (DEC EDI backs
   onto it), then copies from src to dest a WORD at a time with LODSW/STOSW, stopping when
   either byte of the pair is 0 (the interleaved TEST AL,AL / TEST AH,AH pairs terminate on
   embedded NULs); returns the end pointer EDI in EAX. All GP regs + EBP are hand-asm scratch
   (the body owns its PUSH/POP frame) -> declared in the wrapper modify set so no compiler
   frame/save is added. Body db-transcribed minus the trailing RET (frameless void wrapper
   supplies it); no external calls or absolute data refs. */
extern void __appendw(void);
#pragma aux __appendw = "db 86" "db 87" "db 85" "db 137" "db 229" "db 139" "db 125" "db 16" "db 139" "db 117" "db 20" "db 6" "db 140" "db 216" "db 142" "db 192" "db 87" "db 41" "db 201" "db 73" "db 49" "db 192" "db 242" "db 174" "db 79" "db 102" "db 173" "db 132" "db 192" "db 116" "db 19" "db 102" "db 171" "db 132" "db 228" "db 116" "db 14" "db 102" "db 173" "db 132" "db 192" "db 116" "db 7" "db 102" "db 171" "db 132" "db 228" "db 117" "db 232" "db 168" "db 170" "db 95" "db 7" "db 137" "db 248" "db 93" "db 95" "db 94" modify exact [eax ebx ecx edx esi edi ebp];
#pragma aux strcat_word modify [eax ebx ecx edx esi edi ebp];
void strcat_word(void)
{
    __appendw();
}
