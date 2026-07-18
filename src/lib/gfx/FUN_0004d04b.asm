; FUN_0004d04b @ 0004d04b  (30 bytes) -- hand-written assembly (fully commented).
;
; copy_fixed_block: byte-copy a fixed-size block from src to dst. The length is a
; build-time constant computed inline as 0x1c632 - 0x108 = 0x1c52a (116010) bytes,
; so this copies one whole game buffer of that size in a single rep movsb.
;
; Args (stack / cdecl):  [ebp+8] = src   [ebp+0xc] = dst
; Clobbers ecx, esi, edi. The build uses FUN_0004d04b.c (db-transcription); this is
; the readable companion.
;
FUN_0004d04b:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ecx                              ; 51
        push    edi                              ; 57
        push    esi                              ; 56
        mov     ecx, 0x1c632                     ; b932c60100 -- ecx = 0x1c632 ...
        sub     ecx, 0x108                       ; 81e908010000  -- ... - 0x108 = 0x1c52a bytes to copy
        mov     esi, dword ptr [ebp + 8]         ; 8b7508     -- esi = src
        mov     edi, dword ptr [ebp + 0xc]       ; 8b7d0c     -- edi = dst
        rep movsb byte ptr es:[edi], byte ptr [esi] ; f3a4    -- copy the whole block
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3
