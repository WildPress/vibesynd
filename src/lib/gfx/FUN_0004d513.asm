; FUN_0004d513 @ 0004d513  (25 bytes) -- hand-written assembly (fully commented).
;
; hex32_to_ascii: format a 32-bit value as exactly 8 hexadecimal digits.
;
; Writes the digits most-significant first into the buffer at edi, no leading-zero
; suppression and no terminator (the caller null-terminates, e.g. FUN_0004cacc).
; Nibbles are turned into characters through a 16-byte digit table at 0x3fde4
; ("0123456789ABCDEF" or similar), so the exact digit glyphs come from that table.
;
; Registers in:  eax = value to format      edi = output buffer (advanced by 8)
; Clobbers eax, ebx, ecx, edi. Globals: 0x3fde4 hex-digit lookup table.
; The build uses FUN_0004d513.c (db-transcription); this listing is the readable
; companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d513:
        mov     cx, 8                            ; 66b90800   -- 8 nibbles to emit
digit:                                           ;            <- loop top (0x4d517)
        rol     eax, 4                           ; c1c004     -- bring next-highest nibble into the low 4 bits
        mov     ebx, eax                         ; 8bd8       -- copy so we can mask without losing eax
        and     ebx, 0xf                         ; 83e30f     -- ebx = current nibble (0..15)
        mov     bl, byte ptr [ebx + 0x3fde4]     ; 8a9be4fd0300  -- bl = digit char from table[nibble]
        mov     byte ptr [edi], bl               ; 881f       -- store the character
        inc     edi                              ; 47         -- advance output
        loop    0x4d517                          ; 67e2ec     -- dec cx; repeat for all 8 nibbles (-> digit)
        ret                                      ; c3
