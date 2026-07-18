; isqrt16 @ 0004d352  (65 bytes) -- hand-written assembly (fully commented).
;
; isqrt16: integer square root of a 16-bit unsigned value. Identical method to the
; 32-bit isqrt32 but working in 16-bit registers: bsr to find the magnitude,
; a seeded first guess from the table at 0x3fc81, then Newton iterations
; guess = (guess + n/guess) / 2 until n/guess is no longer below guess. Input 0
; returns 0. Returns floor(sqrt(n)) in ax.
;
; Arg (stack / cdecl):  [ebp+8] (word) = n     Returns: ax = floor(sqrt(n))
; Globals: 0x3fc81 = per-bit-length initial-guess seed table (words).
; The build uses isqrt16.c (db-transcription); this is the readable companion.
;
isqrt16:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    bx                               ; 6653
        push    cx                               ; 6651
        push    dx                               ; 6652
        xor     eax, eax                         ; 33c0
        mov     cx, word ptr [ebp + 8]           ; 668b4d08   -- cx = n
        or      cx, cx                           ; 660bc9
        je      0x4d38b                          ; 7425       -- n == 0 -> return 0 (-> ret_zero)
        bsr     ax, cx                           ; 660fbdc1   -- ax = index of highest set bit of n
        mov     bx, word ptr [eax*2 + 0x3fc81]   ; 668b1c4581fc0300  -- bx = seed guess from table[bit]
iterate:                                         ;            <- (0x4d372)
        mov     ax, cx                           ; 668bc1     -- ax = n
        xor     dx, dx                           ; 6633d2
        div     bx                               ; 66f7f3     -- ax = n / guess
        cmp     ax, bx                           ; 663bc3
        jge     0x4d388                          ; 7d08       -- n/guess >= guess -> converged (-> done)
        add     bx, ax                           ; 6603d8     -- guess = guess + n/guess
        shr     bx, 1                            ; 66d1eb     --        = (guess + n/guess) / 2
        jmp     0x4d372                          ; ebea       -- iterate again (-> iterate)
done:                                            ;            <- (0x4d388)
        mov     ax, bx                           ; 668bc3     -- result = guess
ret_zero:                                        ;            <- (0x4d38b) ax already 0 on the zero path
        pop     dx                               ; 665a
        pop     cx                               ; 6659
        pop     bx                               ; 665b
        leave                                    ; c9
        ret                                      ; c3
