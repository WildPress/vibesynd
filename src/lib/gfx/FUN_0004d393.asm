; FUN_0004d393 @ 0004d393  (54 bytes) -- hand-written assembly (fully commented).
;
; isqrt32: integer square root of a 32-bit unsigned value, by Newton's method with
; a table-seeded first guess.
;
; It takes the position of the value's highest set bit (bsr) and looks up a good
; initial guess in a seed table at 0x3fc81 (word entries indexed by bit position).
; Then it iterates the standard step  guess = (guess + n/guess) / 2  until n/guess
; is no longer less than guess, at which point guess is floor(sqrt(n)) and is
; returned in eax. Input 0 returns 0.
;
; The game reaches this through a "return sqrt(a*a + b*b)" wrapper (see the caller
; noted in manifest/names.json for FUN_0004d352), i.e. it computes 2-D vector length.
; The 16-bit sibling is FUN_0004d352.
;
; Arg (stack / cdecl):  [ebp+8] = n        Returns: eax = floor(sqrt(n))
; Globals: 0x3fc81 = per-bit-length initial-guess seed table (words).
; The build uses FUN_0004d393.c (db-transcription); this is the readable companion.
;
FUN_0004d393:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        mov     ecx, dword ptr [ebp + 8]         ; 8b4d08     -- ecx = n
        or      ecx, ecx                         ; 0bc9
        je      0x4d3c2                          ; 7422       -- n == 0 -> return 0 (-> ret_zero)
        bsr     eax, ecx                         ; 0fbdc1     -- eax = index of highest set bit of n
        movzx   ebx, word ptr [eax*2 + 0x3fc81]  ; 0fb71c4581fc0300  -- ebx = seed guess from table[bit]
iterate:                                         ;            <- (0x4d3ab)
        mov     eax, ecx                         ; 8bc1       -- eax = n
        xor     edx, edx                         ; 33d2
        div     ebx                              ; f7f3       -- eax = n / guess
        cmp     eax, ebx                         ; 3bc3
        jge     0x4d3bb                          ; 7d06       -- n/guess >= guess -> converged (-> done)
        add     ebx, eax                         ; 03d8       -- guess = guess + n/guess
        shr     ebx, 1                           ; d1eb       --        = (guess + n/guess) / 2
        jmp     0x4d3ab                          ; ebf0       -- iterate again (-> iterate)
done:                                            ;            <- (0x4d3bb)
        mov     eax, ebx                         ; 8bc3       -- result = guess
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
ret_zero:                                        ;            <- (0x4d3c2)
        xor     eax, eax                         ; 33c0       -- return 0
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
