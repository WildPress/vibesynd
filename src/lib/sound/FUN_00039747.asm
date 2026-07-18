; FUN_00039747 @ 00039747  (48 bytes) -- hand-written assembly (fully commented).
;
; FUN_00039747: start/resume a sequence. If the slot's state word is 1 (loaded/paused),
; bump it to 2 (running); otherwise leave it untouched. The 1->2 transition is what the
; timer ISR keys on to begin servicing the slot. Paired with FUN_0003979c (2->1, pause).
;
; Args (stack, cdecl):  [ebp+8] = slot index
; No return value. Runs inside a cli critical section.
; Globals:  0xbc38 g_seq_state[17] (word)   (0 = free, 1 = loaded/paused, 2 = running)
;
FUN_00039747:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                       ; fa    enter critical section
        mov     ebx, dword ptr [ebp + 8]          ; 8b5d08       ebx = slot index (arg)
        shl     ebx, 1                            ; d1e3        ebx = slot*2 (word index)
        cmp     word ptr [ebx + 0xbc38], 1        ; 6683bb38bc000001  state == 1 (loaded/paused)?
        jne     0x39766                            ; 7509       no -> .exit (leave as is)
        mov     word ptr [ebx + 0xbc38], 2        ; 66c78338bc00000200  state = 2 (running)
; .exit (0x39766): critical-section exit
        push    ebp                               ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x39771                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
