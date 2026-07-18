; dec32_to_ascii @ 0004d451  (39 bytes) -- hand-written assembly (fully commented).
;
; u32_to_ascii_dec: format a 32-bit value as exactly 10 decimal digits.
;
; Classic repeated-subtraction conversion. For each of the ten place values it
; subtracts the corresponding power of ten as many times as it can, counting the
; subtractions into the digit, then restores the remainder and stores the digit as
; ASCII ('0'+n). Emits a fixed 10-character field, most significant first, with
; leading zeros (the caller decides how many trailing digits to actually show, e.g.
; draw_dec). No terminator is written.
;
; Registers in:  eax = value          edi = output buffer (advanced by 10)
; Clobbers eax, ebx, cx, dl, edi. Globals: 0x3fd30 = table of the ten powers of ten
; (dwords: 1e9, 1e8, ... 10, 1). The build uses dec32_to_ascii.c (db-transcription);
; this listing is the readable companion.
;
dec32_to_ascii:
        xor     ebx, ebx                         ; 33db       -- ebx = index into power-of-ten table (0,4,8,...)
        mov     cx, 0xa                           ; 66b90a00   -- 10 digit positions
        xor     dl, dl                            ; 32d2       <- digit loop (0x4d457): dl = current digit = 0
sub_loop:                                        ;            <- (0x4d459)
        sub     eax, dword ptr [ebx + 0x3fd30]    ; 2b8330fd0300  -- try to subtract this place value
        jb      0x4d465                           ; 7204       -- borrow -> too far, undo (-> emit)
        inc     dl                                ; fec2       -- else count one more into the digit
        jmp     0x4d459                           ; ebf4       -- keep subtracting (-> sub_loop)
emit:                                            ;            <- (0x4d465)
        add     eax, dword ptr [ebx + 0x3fd30]    ; 038330fd0300  -- restore the last (over-)subtracted value
        add     dl, 0x30                          ; 80c230     -- digit -> ASCII '0'..'9'
        mov     byte ptr [edi], dl                ; 8817       -- store the character
        inc     edi                               ; 47         -- advance output
        add     ebx, 4                            ; 83c304     -- next power-of-ten table entry
        loop    0x4d457                           ; 67e2e0     -- dec cx; next place (-> digit loop)
        ret                                       ; c3
