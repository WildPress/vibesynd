; save_backing_column @ 0x4a42f  (99 bytes) -- hand-written assembly (fully commented).
;
; save_backing_column: save a 16-pixel-wide, 17-row column from g_screen_buf into the fixed
; scratch area at 0x3cb74. This is the "save" half of a backing-store pair; the
; matching "restore" restore_backing_column copies the same column back the other way.
;
; Structurally identical to restore_backing_column but with source and destination swapped: it
; reads the four plane dwords (offsets 0, 0x7d00, 0xfa00, 0x17700) at the left edge of
; each of 17 rows starting at scanline arg0 and writes them in sequence into the
; linear scratch buffer. If the start row is >= 0x190 (400) it does nothing.
;
; Args (stack / cdecl):
;   [ebp+8]   start row (word); incremented in place as the loop advances
; Globals:  0x5368  g_screen_buf     0x3cb74  scratch backing store (17*16 bytes)
;
save_backing_column:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        mov     esi, dword ptr [0x5368]          ; esi = g_screen_buf
        mov     edi, 0x3cb74                     ; edi = scratch destination
        movzx   eax, word ptr [ebp + 8]          ; eax = start row
        imul    eax, eax, 0x50                   ; row * 0x50 (row pitch)
        add     esi, eax                         ; esi = source at (col 0, row)
        mov     ecx, 0x11                        ; 17 rows
        cmp     word ptr [ebp + 8], 0x190        ; start row >= 400 ?
        jge     done                             ;   yes -> off the buffer, skip
copy_row:
        mov     edx, dword ptr [esi]             ; plane 0 -> scratch
        mov     dword ptr [edi], edx
        add     edi, 4
        mov     edx, dword ptr [esi + 0x7d00]    ; plane 1 -> scratch
        mov     dword ptr [edi], edx
        add     edi, 4
        mov     edx, dword ptr [esi + 0xfa00]    ; plane 2 -> scratch
        mov     dword ptr [edi], edx
        add     edi, 4
        mov     edx, dword ptr [esi + 0x17700]   ; plane 3 -> scratch
        mov     dword ptr [edi], edx
        add     edi, 4
        add     esi, 0x50                        ; next screen row
        inc     word ptr [ebp + 8]               ; advance row counter
        loop    copy_row
done:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
