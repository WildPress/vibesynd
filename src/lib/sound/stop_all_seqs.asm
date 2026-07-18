; stop_all_seqs @ 00039722  (37 bytes) -- hand-written assembly (fully commented).
;
; stop_all_seqs: stop every sequence. Loop slot index from 15 down to 0 and call
; free_seq_slot (release slot) on each, so all voices are freed and -- once the count
; hits zero inside the last call -- the timer subsystem is torn down too. Called at
; the end of the driver-teardown path (unload_all_drivers).
;
; No args, no return value. Runs inside a cli critical section.
; Calls:  0x396d5 free_seq_slot(slot)
;
stop_all_seqs:
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                      ; fa    enter critical section
        mov     esi, 0xf                          ; be0f000000   slot = 15
; .loop (0x3972b):
        push    esi                              ; 56           arg = slot
        call    0x396d5                          ; e8a4ffffff  -> free_seq_slot(slot): release it
        add     esp, 4                            ; 83c404
        dec     esi                               ; 4e          slot--
        jge     0x3972b                           ; 7df4        slot >= 0 -> .loop
; --- critical-section exit ---
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x39742                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        ret                                      ; c3
