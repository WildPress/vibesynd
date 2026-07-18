; FUN_00039879 @ 00039879  (69 bytes) -- hand-written assembly (fully commented).
;
; FUN_00039879: compute a sequence period from a rate argument and arm the slot with it
; (via FUN_000397f1). If the rate is 0 the period is set to the 0xd68d us sentinel (the
; slowest tick / "off"). Otherwise period = arg * 10000 / 11932 us.
;
; The 10000/11932 (= 0.8381) scale is approximately the reciprocal of the us->PIT-count
; scale (10000/8380 = 1.1933) that FUN_00039495 applies when the PIT is programmed, so
; `arg` appears to be a raw PIT reload count being converted back into a microsecond
; period. (Purpose of the arg is inferred from that reciprocal relationship.)
;
; Args (stack, cdecl):  [ebp+8] = slot index   [ebp+0xc] = rate value (0 = off)
; No return value. Runs inside a cli critical section.
; Calls:  0x397f1 FUN_000397f1(slot, period_us)
;
FUN_00039879:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                      ; fa    enter critical section
        cmp     dword ptr [ebp + 0xc], 0          ; 837d0c00     rate == 0 ?
        jne     0x3988d                            ; 7507       no -> .convert
        mov     eax, 0xd68d                        ; b88dd60000  yes -> period = 0xd68d us (off/slowest)
        jmp     0x398a1                            ; eb14       -> .arm
; .convert (0x3988d): period = rate * 10000 / 11932
        mov     eax, 0x2710                        ; b810270000  eax = 10000
        mov     edx, 0                             ; ba00000000  clear edx for the multiply/divide
        mov     ebx, 0x2e9c                         ; bb9c2e0000 ebx = 11932 (divisor)
        mul     dword ptr [ebp + 0xc]              ; f7650c      edx:eax = 10000 * rate
        div     ebx                                ; f7f3        eax = 10000 * rate / 11932 (period us)
; .arm (0x398a1):
        push    eax                                ; 50          arg2 = period_us
        push    dword ptr [ebp + 8]                ; ff7508      arg1 = slot index
        call    0x397f1                            ; e847ffffff -> FUN_000397f1(slot, period)
        add     esp, 8                             ; 83c408
; --- critical-section exit ---
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x398b8                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3
