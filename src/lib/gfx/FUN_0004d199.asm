; FUN_0004d199 @ 0x4d199  (66 bytes) -- hand-written assembly (fully commented).
;
; fill_bytes: a size-optimised memset. Fill `count` bytes at `dst` with `value`.
; It picks the widest store the count allows, so most of the work goes four bytes
; at a time instead of one: count divisible by 4 -> rep stosd, divisible by 2 ->
; rep stosw, otherwise -> rep stosb. (value is passed as a full dword, so its low
; byte/word is what lands in the narrower stores.)
;
; Called for plain clears, e.g. FUN_0004d199(g_5594, 0, 0x1e9) zeroes a 0x1e9-byte
; state block. Args (stack / cdecl): [ebp+8] dst, [ebp+0xc] value, [ebp+0x10] count.

FUN_0004d199:
        push    ebp
        mov     ebp, esp
        push    ecx
        push    edi
        mov     ecx, [ebp+0x10]     ; ecx = count
        test    ecx, 3
        je      fill_dword          ; count % 4 == 0 -> dword path
        test    ecx, 2
        je      fill_word           ; count % 2 == 0 (but not 4) -> word path

; --- byte fill (odd count) ---
        mov     edi, [ebp+8]        ; edi = dst
        mov     eax, [ebp+0xc]      ; al = value
        rep stosb                   ; store count bytes
        pop     edi
        pop     ecx
        leave
        ret

fill_word:
        shr     ecx, 1              ; count / 2 words
        mov     edi, [ebp+8]
        mov     eax, [ebp+0xc]      ; ax = value
        rep stosw
        pop     edi
        pop     ecx
        leave
        ret

fill_dword:
        shr     ecx, 2              ; count / 4 dwords
        mov     edi, [ebp+8]
        mov     eax, [ebp+0xc]      ; eax = value
        rep stosd
        pop     edi
        pop     ecx
        leave
        ret
