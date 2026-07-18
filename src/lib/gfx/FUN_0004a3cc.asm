; FUN_0004a3cc @ 0x4a3cc  (99 bytes) -- hand-written assembly (fully commented).
;
; FUN_0004a3cc: restore a 16-pixel-wide, 17-row column into g_screen_buf from a fixed
; scratch save area at 0x3cb74. This is the "restore" half of a backing-store pair;
; FUN_0004a42f is the matching "save" that copies the same column the other way.
;
; The destination row is arg0 (a scanline, word). It walks down 17 rows at the left
; edge of the buffer (esi = g_screen_buf + row*0x50), and for each row copies four
; dwords -- one per plane, at buffer offsets 0, 0x7d00, 0xfa00, 0x17700 -- pulled in
; sequence from the linear scratch buffer. 16 bytes/row across 4 planes = 16 pixels
; wide. If the start row is >= 0x190 (400, past the buffer bottom) it does nothing.
;
; Args (stack / cdecl):
;   [ebp+8]   start row (word); incremented in place as the loop advances
; Globals:  0x5368  g_screen_buf     0x3cb74  scratch backing store (17*16 bytes)
;
FUN_0004a3cc:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        mov     esi, dword ptr [0x5368]          ; esi = g_screen_buf
        mov     edi, 0x3cb74                     ; edi = scratch source
        movzx   eax, word ptr [ebp + 8]          ; eax = start row
        imul    eax, eax, 0x50                   ; row * 0x50 (row pitch)
        add     esi, eax                         ; esi = dest at (col 0, row)
        mov     ecx, 0x11                        ; 17 rows
        cmp     word ptr [ebp + 8], 0x190        ; start row >= 400 ?
        jge     done                             ;   yes -> off the buffer, skip
copy_row:
        mov     edx, dword ptr [edi]             ; scratch -> plane 0
        mov     dword ptr [esi], edx
        add     edi, 4
        mov     edx, dword ptr [edi]             ; scratch -> plane 1
        mov     dword ptr [esi + 0x7d00], edx
        add     edi, 4
        mov     edx, dword ptr [edi]             ; scratch -> plane 2
        mov     dword ptr [esi + 0xfa00], edx
        add     edi, 4
        mov     edx, dword ptr [edi]             ; scratch -> plane 3
        mov     dword ptr [esi + 0x17700], edx
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
