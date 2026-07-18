; FUN_0004d0b4 @ 0x4d0b4  (229 bytes) -- hand-written assembly (fully commented).
;
; blit_block: copy a rectangular block of pixels from the back buffer to the
; screen buffer, across all four mode-X planes.
;
; Both buffers use the planar mode-X layout: four planes 0x7d00 (32000) bytes
; apart, each plane a grid of 80-byte (20-dword) rows. Any one pixel lives in a
; single plane, so copying the same rectangle in all four planes moves the whole
; colour image. The routine copies the rectangle once, then adds 0x7d00 to both
; pointers and copies it again, four times over.
;
; Args (stack / cdecl), in the planar coordinate system:
;   [ebp+8]   src x    (dwords across a row -> *4 bytes)
;   [ebp+0xc] src y    (in 16-row bands -> *0x500)
;   [ebp+0x10]dst x    (dwords)
;   [ebp+0x14]dst y    (16-row bands)
;   [ebp+0x18]width    (dwords per row)
;   [ebp+0x1c]height   (16-row bands -> *16 rows)
;
; Row pitch is 20 dwords (80 bytes); after copying `width` dwords the pointers skip
; (20 - width) dwords to the next row. Runs when render-mode bit 1 (0x105) is set.
; Saves every register. The push/pop around each plane loop preserves the rectangle
; origin so the next plane restarts from the same spot, one plane deeper.
;
; Globals:  0x5370  g_back_buf (source)
;           0x5368  g_screen_buf (dest)
;           0x105   render-mode flags

FUN_0004d0b4:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        test    byte [0x105], 2
        je      done                ; only blit in this render mode

        mov     eax, [ebp+8]
        shl     eax, 2              ; src_x * 4  (dword -> byte)
        imul    esi, [ebp+0xc], 0x500   ; src_y * 0x500 (one 16-row band)
        add     esi, eax
        mov     eax, [ebp+0x10]
        shl     eax, 2              ; dst_x * 4
        imul    edi, [ebp+0x14], 0x500  ; dst_y * 0x500
        add     edi, eax
        add     esi, [0x5370]       ; + g_back_buf    -> source pointer
        add     edi, [0x5368]       ; + g_screen_buf  -> dest pointer
        mov     ebx, [ebp+0x1c]
        shl     ebx, 4              ; height: bands -> rows (*16)
        mov     eax, [ebp+0x18]     ; eax = width in dwords
        mov     edx, 0x14
        sub     edx, eax            ; 20 - width
        shl     edx, 2              ; (20 - width) * 4 = per-row skip in bytes

; --- plane 0 ---
        push    esi                 ; save the rectangle origin for the next plane
        push    edi
        push    ebx
p0row:  mov     ecx, eax            ; ecx = width dwords
        rep movsd                   ; copy one row
        add     esi, edx            ; skip to next source row
        add     edi, edx            ; skip to next dest row
        dec     ebx
        jg      p0row
        pop     ebx
        pop     edi
        pop     esi
        add     edi, 0x7d00         ; step both pointers to plane 1
        add     esi, 0x7d00

; --- plane 1 ---
        push    esi
        push    edi
        push    ebx
p1row:  mov     ecx, eax
        rep movsd
        add     esi, edx
        add     edi, edx
        dec     ebx
        jg      p1row
        pop     ebx
        pop     edi
        pop     esi
        add     edi, 0x7d00         ; -> plane 2
        add     esi, 0x7d00

; --- plane 2 ---
        push    esi
        push    edi
        push    ebx
p2row:  mov     ecx, eax
        rep movsd
        add     esi, edx
        add     edi, edx
        dec     ebx
        jg      p2row
        pop     ebx
        pop     edi
        pop     esi
        add     edi, 0x7d00         ; -> plane 3
        add     esi, 0x7d00

; --- plane 3 ---
        push    esi
        push    edi
        push    ebx
p3row:  mov     ecx, eax
        rep movsd
        add     esi, edx
        add     edi, edx
        dec     ebx
        jg      p3row
        pop     ebx
        pop     edi
        pop     esi
        add     edi, 0x7d00
        add     esi, 0x7d00
        jmp     done

; render-mode dispatch tail (other modes do nothing extra here)
        test    byte [0x105], 4
        je      $+4
        jmp     done
        test    byte [0x105], 1
        je      done

done:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
