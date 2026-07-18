; FUN_0004d442 @ 0004d442  (15 bytes) -- hand-written assembly (fully commented).
;
; poll_key: non-blocking keyboard read through the PC BIOS. It first asks the BIOS
; whether a keystroke is waiting (INT 16h, AH=1, which sets ZF when the buffer is
; empty). If one is waiting it fetches it (INT 16h, AH=0) and returns it in AL;
; otherwise it returns AL=0. AH holds the scan code, AL the ASCII/character byte.
;
; Args: none.   Returns: al = key (0 if no key pending).
; Ports/BIOS: INT 16h (keyboard services). The build uses FUN_0004d442.c
; (db-transcription); this is the readable companion.
;
FUN_0004d442:
        mov     ah, 1                            ; b401       -- BIOS: peek keyboard buffer
        int     0x16                             ; cd16       -- ZF=1 if empty, ZF=0 if a key waits
        je      0x4d44e                          ; 7406       -- no key -> return 0 (-> no_key)
        mov     ah, 0                            ; b400       -- BIOS: read/remove next key
        int     0x16                             ; cd16       -- ah=scan code, al=character
        jmp     0x4d450                          ; eb02       -- return with the key (-> ret)
no_key:                                          ;            <- (0x4d44e)
        mov     al, 0                            ; b000       -- al = 0 (no key)
ret:                                             ;            <- (0x4d450)
        ret                                      ; c3
