/* @ 0x3bd31 (6B) -- db-transcription: the shared function epilogue of doprnt
   (leave; pop es; pop edi; pop esi; pop ebx; ret). prtf_conversion @0x3c019 ends with
   a byte-saving tail  into this epilogue (its frame matches doprnt's at
   that point). Carved as a standalone copy so the FUN_0003bd31 tail-jump resolves to a
   real, frame-neutral stack-cleanup+ret instead of a ret-stub. No calls, no data refs. */
extern void __db_FUN_0003bd31_0(void);
#pragma aux __db_FUN_0003bd31_0 = "db 201" "db 7" "db 95" "db 94" "db 91" "db 195" modify exact [eax ebx ecx edx esi edi];
#pragma aux FUN_0003bd31 modify [eax ebx ecx edx esi edi] aborts;
void FUN_0003bd31(void) { __db_FUN_0003bd31_0(); }
