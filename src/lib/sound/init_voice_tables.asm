; init_voice_tables @ 0x3954c  (106 bytes) -- hand-written assembly (fully commented).
;
; init_voice_tables: reset the per-voice driver state to "empty" at driver start-up.
; It clears three parallel 16-entry dword tables and a couple of scalar cells, with
; interrupts disabled so a timer tick can't observe a half-initialised table.
;
; The three tables (each 16 dwords, indexed by voice 0..15):
;   0xbcfa  g_voice_driver[16]  driver dispatch-table pointer per voice  -> 0
;   0xbd3a  g_voice_handle[16]  per-voice handle                          -> 0xffffffff (none)
;   0xbd7a  g_voice_active[16]  per-voice active flag                     -> 0 (idle)
; The -1 fill for g_voice_handle matches the "-1 = no handle" convention the play/stop
; paths (FUN_000399bd / stop_voice) test against.
;
; Also: 0xbdca is loaded with DS and copied into ES -- caching the data selector the ISR
; uses for its string ops; 0xbbf0/0xbbf2 are two state words zeroed here.

init_voice_tables:
        push    esi
        push    edi
        push    es
        pushfd                               ; save flags (incl. IF)
        cli                                  ; critical section
        mov     word ptr [0xbdca], ds        ; cache the data selector...
        mov     es, word ptr [0xbdca]        ; ...and load it into ES for the stos runs
        mov     word ptr [0xbbf0], 0         ; clear state word
        mov     word ptr [0xbbf2], 0         ; clear state word
        cld
        mov     edi, 0xbcfa                  ; g_voice_driver[]
        mov     ecx, 0x10                    ; 16 entries
        mov     eax, 0
        rep stosd dword ptr es:[edi], eax    ;   = 0 (no driver)
        mov     edi, 0xbd3a                  ; g_voice_handle[]
        mov     ecx, 0x10
        mov     eax, 0xffffffff
        rep stosd dword ptr es:[edi], eax    ;   = -1 (no handle)
        mov     edi, 0xbd7a                  ; g_voice_active[]
        mov     ecx, 0x10
        mov     eax, 0
        rep stosd dword ptr es:[edi], eax    ;   = 0 (idle)
        push    ebp                          ; --- restore caller's interrupt state, then flags ---
        mov     ebp, esp
        test    byte ptr [ebp + 5], 2        ; saved EFLAGS bit 9 (IF) set on entry?
        cli
        je      flags_done
        sti
flags_done:
        pop     ebp
        popfd
        pop     es
        pop     edi
        pop     esi
        ret
