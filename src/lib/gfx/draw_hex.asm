; draw_hex @ 0004cacc  (78 bytes) -- hand-written assembly (fully commented).
;
; draw_hex: format a 32-bit value as hexadecimal and draw the last `ndigits` digits
; as text. It writes 8 hex digits plus a terminator into the scratch buffer at 0xe384
; (via hex32_to_ascii), then points at the right-justified tail of that buffer -- the
; last `ndigits` characters -- and hands it, with the caller's font/x/y/colour, to the
; proportional text renderer draw_text.
;
; The string pointer is computed as 0xe384 + 8 - ndigits, i.e. the start of the last
; ndigits of the 8-digit field, so leading digits are simply not drawn.
;
; Args (stack / cdecl):  [ebp+8] value   [ebp+0xc] font table   [ebp+0x10] x (word)
;                        [ebp+0x14] y (word)   [ebp+0x18] colour (word)
;                        [ebp+0x1c] ndigits (word)
; Globals: 0xe384 scratch text buffer. The build uses draw_hex.c
; (db-transcription); this is the readable companion.
;
draw_hex:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        mov     eax, dword ptr [ebp + 8]         ; 8b4508     -- eax = value
        mov     edi, 0xe384                      ; bf84e30000 -- edi = scratch buffer
        call    0x4d513                          ; e8320a0000  -- hex32_to_ascii: write 8 hex digits (edi += 8)
        mov     byte ptr [edi], 0                ; c60700     -- null-terminate after the 8 digits
        movzx   eax, word ptr [ebp + 0x1c]       ; 0fb7451c   -- eax = ndigits (digits to show)
        neg     eax                              ; f7d8
        add     eax, 0xe384                      ; 0584e30000 -- eax = 0xe384 - ndigits ...
        add     eax, 8                           ; 83c008     -- ... + 8  -> start of last ndigits
        sub     esp, 2                           ; 83ec02     -- (align the word push below)
        push    word ptr [ebp + 0x18]            ; 66ff7518   -- arg: colour
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x14]            ; 66ff7514   -- arg: y
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x10]            ; 66ff7510   -- arg: x
        push    dword ptr [ebp + 0xc]            ; ff750c     -- arg: font table
        push    eax                              ; 50         -- arg: string pointer
        call    0x4cb68                          ; e858000000  -- draw_text: draw the text
        add     esp, 0x14                        ; 83c414     -- drop the 5 argument slots
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
