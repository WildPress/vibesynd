; FUN_0004cb1a @ 0004cb1a  (78 bytes) -- hand-written assembly (fully commented).
;
; draw_dec: the decimal twin of draw_hex (FUN_0004cacc). It formats a 32-bit value as
; a 10-digit decimal string in the scratch buffer at 0xe384 (via FUN_0004d451), then
; draws the right-justified last `ndigits` characters through the proportional text
; renderer FUN_0004cb68. The string pointer is 0xe384 + 0xa - ndigits, so the leading
; zeros of the fixed 10-digit field are simply not drawn.
;
; Args (stack / cdecl):  [ebp+8] value   [ebp+0xc] font table   [ebp+0x10] x (word)
;                        [ebp+0x14] y (word)   [ebp+0x18] colour (word)
;                        [ebp+0x1c] ndigits (word)
; Globals: 0xe384 scratch text buffer. The build uses FUN_0004cb1a.c
; (db-transcription); this is the readable companion.
;
FUN_0004cb1a:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        mov     eax, dword ptr [ebp + 8]         ; 8b4508     -- eax = value
        mov     edi, 0xe384                      ; bf84e30000 -- edi = scratch buffer
        call    0x4d451                          ; e822090000  -- FUN_0004d451: write 10 decimal digits (edi += 10)
        mov     byte ptr [edi], 0                ; c60700     -- null-terminate after the 10 digits
        movzx   eax, word ptr [ebp + 0x1c]       ; 0fb7451c   -- eax = ndigits (digits to show)
        neg     eax                              ; f7d8
        add     eax, 0xe384                      ; 0584e30000 -- eax = 0xe384 - ndigits ...
        add     eax, 0xa                         ; 83c00a     -- ... + 10 -> start of last ndigits
        sub     esp, 2                           ; 83ec02     -- (align the word push below)
        push    word ptr [ebp + 0x18]            ; 66ff7518   -- arg: colour
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x14]            ; 66ff7514   -- arg: y
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x10]            ; 66ff7510   -- arg: x
        push    dword ptr [ebp + 0xc]            ; ff750c     -- arg: font table
        push    eax                              ; 50         -- arg: string pointer
        call    0x4cb68                          ; e80a000000  -- FUN_0004cb68: draw the text
        add     esp, 0x14                        ; 83c414     -- drop the 5 argument slots
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
