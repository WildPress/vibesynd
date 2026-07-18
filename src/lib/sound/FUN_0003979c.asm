; FUN_0003979c @ 0003979c  (48 bytes) -- hand-written assembly (fully commented).
;
; FUN_0003979c: pause a sequence. If the slot's state word is 2 (running), drop it back
; to 1 (loaded/paused); otherwise leave it untouched. This is the exact inverse of
; FUN_00039747 (1->2, start/resume): the timer ISR stops servicing a slot once it is
; no longer in state 2.
;
; Args (stack, cdecl):  [ebp+8] = slot index
; No return value. Runs inside a cli critical section.
; Globals:  0xbc38 g_seq_state[17] (word)   (0 = free, 1 = loaded/paused, 2 = running)
;
FUN_0003979c:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                       ; fa    enter critical section
        mov     ebx, dword ptr [ebp + 8]          ; 8b5d08       ebx = slot index (arg)
        shl     ebx, 1                            ; d1e3        ebx = slot*2 (word index)
        cmp     word ptr [ebx + 0xbc38], 2        ; 6683bb38bc000002  state == 2 (running)?
        jne     0x397bb                            ; 7509       no -> .exit (leave as is)
        mov     word ptr [ebx + 0xbc38], 1        ; 66c78338bc00000100  state = 1 (loaded/paused)
; .exit (0x397bb): critical-section exit
        push    ebp                               ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x397c6                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
