; unload_all_drivers @ 000395b6  (111 bytes) -- hand-written assembly (fully commented).
;
; unload_all_drivers: driver teardown -- walk all 16 driver slots and, for every registered
; one, stop its running sequence and release the slot, then stop every remaining
; sequence. Appears to be the "shut everything down" entry (unload all drivers/voices).
;
; For each slot i in 0..15:
;   ptr    = g_driver_ptr[i]  (0xbcfa) -- skip the slot if it is empty (ptr == 0)
;   handle = g_driver_seq[i]  (0xbd3a) -- if not -1, free_seq_slot(handle) stops that seq
;   stop_voice(i) releases the slot (clears its busy flag and dispatches a driver
;                   "free" message)
; Then stop_all_seqs() stops all 16 sequences.
;
; Uses 0xbdbe as a plain loop counter. Args: [ebp+8] -- passed on to stop_voice,
; which reads only its own (slot) argument, so this second value is effectively unused
; by the callee. Runs inside a cli critical section.
;
; Globals:  0xbcfa g_driver_ptr[16]   0xbd3a g_driver_seq[16]   0xbdbe loop counter
; Calls:    0x396d5 free_seq_slot (stop seq)   0x39a82 stop_voice (free slot)
;           0x39722 stop_all_seqs (stop all seqs)
;
unload_all_drivers:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                       ; fa    enter critical section
        mov     dword ptr [0xbdbe], 0             ; c705bebd000000000000  i = 0
; .loop (0x395c7):
        mov     esi, dword ptr [0xbdbe]           ; 8b35bebd0000  esi = i
        shl     esi, 2                            ; c1e602        esi = i*4 (dword index)
        mov     edx, dword ptr [esi + 0xbd3a]     ; 8b963abd0000  edx = g_driver_seq[i] (handle)
        mov     eax, dword ptr [esi + 0xbcfa]     ; 8b86fabc0000  eax = g_driver_ptr[i]
        cmp     eax, 0                            ; 83f800        slot empty?
        je      0x39600                           ; 741f          yes -> .next (skip)
        cmp     edx, -1                           ; 83faff        handle == none (-1)?
        je      0x395ef                           ; 7409          yes -> .free (skip the stop)
        push    edx                               ; 52            arg = handle
        call    0x396d5                           ; e8e9000000   -> free_seq_slot(handle): stop seq
        add     esp, 4                            ; 83c404
; .free (0x395ef): release the driver slot
        push    dword ptr [ebp + 8]              ; ff7508        (extra arg, ignored by callee)
        push    dword ptr [0xbdbe]               ; ff35bebd0000  arg = slot index i
        call    0x39a82                           ; e885040000   -> stop_voice(i): free slot
        add     esp, 8                            ; 83c408
; .next (0x39600):
        inc     dword ptr [0xbdbe]               ; ff05bebd0000  i++
        cmp     dword ptr [0xbdbe], 0x10          ; 833dbebd000010  i == 16 ?
        jne     0x395c7                           ; 75b8          no -> .loop
        call    0x39722                           ; e80e010000   -> stop_all_seqs(): stop all seqs
; --- critical-section exit ---
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x3961f                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
