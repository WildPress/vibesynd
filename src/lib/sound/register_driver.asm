; register_driver @ 000398d7  (143 bytes) -- hand-written assembly (fully commented).
;
; register_driver: register a driver/timbre record into a free driver slot. Find the first
; empty entry in g_driver_ptr[0..15] (0xbcfa), verify the caller's record carries the
; expected signature, store the record's driver pointer in the slot, then probe the
; driver with dispatch message 0x64 (via voice_get_driver_obj) and sanity-check what it returns.
; On success returns the slot index; any failure returns -1.
;
; Args (stack, cdecl):  [ebp+8] = pointer to a driver record
;   record.dword[0] = driver pointer to register
;   record.dword[1] = signature, must equal 0x79706f43 (ASCII "Copy" -- looks like the
;                     start of a "Copyright" tag in the driver header)
; Returns:  eax = slot index (0..15) on success, or -1 (0xffffffff) on any failure.
;
; Globals:  0xbcfa g_driver_ptr[16]   0xbdcc g_bdcc (a ceiling the probe result is checked against)
;           0xbdbe loop counter
; Calls:    0x39994 voice_get_driver_obj(slot) -- dispatch message 0x64 to the newly registered driver
;
register_driver:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                      ; fa    enter critical section
        mov     dword ptr [0xbdbe], 0             ; c705bebd000000000000  i = 0
; .find (0x398e8): scan for a free driver slot (ptr == 0)
        mov     esi, dword ptr [0xbdbe]           ; 8b35bebd0000  esi = i
        shl     esi, 1                            ; d1e6        esi = i*2
        shl     esi, 1                            ; d1e6        esi = i*4 (dword index)
        mov     eax, dword ptr [esi + 0xbcfa]     ; 8b86fabc0000  eax = g_driver_ptr[i]
        cmp     eax, 0                            ; 83f800       slot free?
        je      0x39913                            ; 7416       yes -> .register
        inc     dword ptr [0xbdbe]                 ; ff05bebd0000  i++
        cmp     dword ptr [0xbdbe], 0x10           ; 833dbebd000010  i == 16 ?
        jne     0x398e8                            ; 75dc        no -> .find
        mov     eax, 0xffffffff                    ; b8ffffffff  no free slot -> -1
        jmp     0x39955                            ; eb42       -> .exit
; .register (0x39913): esi = slot*4, validate and install the record
        mov     edi, dword ptr [ebp + 8]          ; 8b7d08       edi = record pointer (arg)
        mov     eax, 0xffffffff                    ; b8ffffffff  default result = -1 (fail)
        cmp     dword ptr [edi + 4], 0x79706f43    ; 817f04436f7079  record.sig == "Copy"?
        jne     0x39955                            ; 7531        no -> .exit (reject, eax=-1)
        mov     edi, dword ptr [edi]               ; 8b3f        edi = record.driver_ptr
        mov     dword ptr [esi + 0xbcfa], edi      ; 89befabc0000  g_driver_ptr[i] = driver ptr
        push    dword ptr [0xbdbe]                 ; ff35bebd0000  arg = slot index i
        call    0x39994                            ; e85d000000  -> voice_get_driver_obj(i): dispatch msg 0x64
        add     esp, 4                             ; 83c404
        mov     edi, eax                           ; 8bf8        edi = probe result pointer
        cmp     eax, 0                             ; 83f800      probe returned null?
        mov     eax, 0xffffffff                    ; b8ffffffff  (preload fail result, flags intact)
        je      0x39955                            ; 740f        null -> .exit (fail)
        mov     edx, dword ptr [edi]               ; 8b17        edx = *probe_result
        cmp     edx, dword ptr [0xbdcc]            ; 3b15ccbd0000  result value > g_bdcc?
        ja      0x39955                            ; 7705        too big -> .exit (fail, eax=-1)
        mov     eax, dword ptr [0xbdbe]            ; a1bebd0000   success -> eax = slot index
; .exit (0x39955): critical-section exit; eax = slot index or -1
        push    ebp                               ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x39960                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
