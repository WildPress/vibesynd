; copy_bytes @ 0004d1db  (70 bytes) -- hand-written assembly (fully commented).
;
; copy_bytes: a size-optimised memcpy. Copy `count` bytes from src to dst, using the
; widest move the count allows so most of the work goes four bytes at a time:
; count divisible by 4 -> rep movsd, divisible by 2 -> rep movsw, otherwise rep movsb.
; It is the copy twin of fill_bytes (FUN_0004d199), which does the same size trick for
; a fill. There is one epilogue per width, so each path is a straight rep-move.
;
; Args (stack / cdecl):  [ebp+8] = src   [ebp+0xc] = dst   [ebp+0x10] = count
; Clobbers ecx, esi, edi. The build uses copy_bytes.c (db-transcription); this is
; the readable companion.
;
copy_bytes:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ecx                              ; 51
        push    edi                              ; 57
        push    esi                              ; 56
        mov     ecx, dword ptr [ebp + 0x10]      ; 8b4d10     -- ecx = count
        test    ecx, 3                           ; f7c103000000
        je      0x4d211                          ; 7425       -- count % 4 == 0 -> dword path (-> copy_dword)
        test    ecx, 2                           ; f7c102000000
        je      0x4d201                          ; 740d       -- count % 2 == 0 (not 4) -> word path (-> copy_word)

; --- byte copy (odd count) ---
        mov     esi, dword ptr [ebp + 8]         ; 8b7508     -- esi = src
        mov     edi, dword ptr [ebp + 0xc]       ; 8b7d0c     -- edi = dst
        rep movsb byte ptr es:[edi], byte ptr [esi] ; f3a4    -- copy count bytes
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3

copy_word:                                       ;            <- (0x4d201)
        shr     ecx, 1                           ; d1e9       -- count / 2 words
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     edi, dword ptr [ebp + 0xc]       ; 8b7d0c
        rep movsw word ptr es:[edi], word ptr [esi] ; f366a5  -- copy count/2 words
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3

copy_dword:                                      ;            <- (0x4d211)
        shr     ecx, 2                           ; c1e902     -- count / 4 dwords
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     edi, dword ptr [ebp + 0xc]       ; 8b7d0c
        rep movsd dword ptr es:[edi], dword ptr [esi] ; f3a5  -- copy count/4 dwords
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3
