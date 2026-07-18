; FUN_00039625 @ 00039625  (176 bytes) -- hand-written assembly (fully commented).
;
; FUN_00039625: allocate a sequence slot. Find the first free entry in the per-sequence
; state table g_seq_state[0..15], mark it active, record the caller's sequence-data
; pointer, and return the slot index (or -1 if all 16 are busy). On the very first
; allocation it also brings the timer subsystem up: clear the voice tables, install the
; IRQ0 ISR, and arm+start the "master" metronome slot 16 at the base period 0xd68d us.
;
; Args (stack, cdecl):  [ebp+8] = pointer to the sequence's data/stream
; Returns:  eax = slot index (0..15), or -1 (0xffffffff) if no slot is free
;
; State table entries: 0 = free, 1 = loaded/paused, 2 = running (see FUN_00039747/9c).
; Slot 16 (g_seq_state[16] at 0xbc58) is the always-on master tick.
;
; Globals:  0xbc38 g_seq_state[17] (word)   0xbbf4 g_seq_data[16] (dword ptr)
;           0xbc9e g_seq_period[17] (dword, us; -1 = none)   0xbbf0 g_active_seq_count
;           0xbc58 g_seq_state[16] (master slot)
; Calls:    0x39393 clear_voice_tables   0x393e0 install_timer_isr
;           0x397f1 FUN_000397f1 (arm seq)   0x39747 FUN_00039747 (start seq)
;
FUN_00039625:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                       ; fa    enter critical section
        mov     ebx, 0                            ; bb00000000   ebx = 0 (byte offset = slot*2)
; .find (0x39631): scan the state table for a free (0) word
        cmp     word ptr [ebx + 0xbc38], 0        ; 6683bb38bc000000  g_seq_state[slot] == 0 ?
        je      0x3964a                           ; 740f         free -> .alloc
        add     ebx, 2                            ; 83c302       next slot (word stride)
        cmp     ebx, 0x20                          ; 83fb20      scanned all 16?
        jb      0x39631                            ; 72ee        no -> .find
        mov     eax, 0xffffffff                    ; b8ffffffff  none free -> return -1
        jmp     0x396c4                            ; eb7a       -> .epilogue
; .alloc (0x3964a): ebx = slot*2, claim the slot
        mov     eax, ebx                          ; 8bc3        eax = slot*2
        shr     eax, 1                            ; d1e8        eax = slot index (return value)
        mov     word ptr [ebx + 0xbc38], 1        ; 66c78338bc00000100  g_seq_state[slot] = 1 (active)
        mov     esi, dword ptr [ebp + 8]          ; 8b7508      esi = sequence data pointer (arg)
        shl     ebx, 1                            ; d1e3        ebx = slot*4 (dword index)
        mov     dword ptr [ebx + 0xbbf4], esi     ; 89b3f4bb0000  g_seq_data[slot] = data ptr
        mov     dword ptr [ebx + 0xbc9e], 0xffffffff ; c7839ebc0000ffffffff  g_seq_period[slot] = none
        inc     word ptr [0xbbf0]                 ; 66ff05f0bb0000  g_active_seq_count++
        cmp     word ptr [0xbbf0], 1              ; 66833df0bb000001  was this the first active seq?
        jne     0x396c4                            ; 7547       no -> .epilogue (subsystem already up)
; --- first activation: start the timer subsystem ---
        push    eax                               ; 50          save our slot index
        call    0x39393                           ; e810fdffff -> clear_voice_tables (wipes state table)
        mov     word ptr [0xbc58], 1              ; 66c70558bc00000100  g_seq_state[16] (master) = 1
        call    0x393e0                           ; e84ffdffff -> install_timer_isr (hook INT 8)
        push    0xd68d                            ; 688dd60000  master period = 0xd68d us (base tick)
        push    0x10                              ; 6a10        master slot = 16
        call    0x397f1                           ; e854010000 -> FUN_000397f1(16, 0xd68d): arm master
        add     esp, 8                            ; 83c408
        push    0x10                              ; 6a10        master slot = 16
        call    0x39747                           ; e8a0000000 -> FUN_00039747(16): start master (1->2)
        add     esp, 4                            ; 83c404
        pop     eax                               ; 58          restore our slot index
; re-apply our slot's state/period (clear_voice_tables wiped them above)
        mov     ebx, eax                          ; 8bd8        ebx = slot index
        shl     ebx, 1                            ; d1e3        ebx = slot*2
        mov     word ptr [ebx + 0xbc38], 1        ; 66c78338bc00000100  g_seq_state[slot] = 1
        shl     ebx, 1                            ; d1e3        ebx = slot*4
        mov     dword ptr [ebx + 0xbc9e], 0xffffffff ; c7839ebc0000ffffffff  g_seq_period[slot] = none
; .epilogue (0x396c4): critical-section exit, eax = slot index or -1
        push    ebp                               ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x396cf                          ; 7401
        sti                                      ; fb
        pop     ebp                               ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
