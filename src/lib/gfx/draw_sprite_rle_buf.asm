; draw_sprite_rle_buf @ 0x4a6c8  (54 bytes) -- hand-written assembly (fully commented).
;
; draw_sprite_rle_buf: draw an RLE (transparent) sprite from a descriptor into the screen
; buffer. A wrapper that unpacks the descriptor into the register convention of
; draw_sprite_rle (0x4a734) and calls it.
;
; The descriptor (pointer in arg2) is: +0 = RLE pixel stream, +4 = width byte,
; +5 = height byte. draw_sprite_rle halves x/y/width/height on entry, so the x/y
; passed here are in the doubled coordinate system it expects.
;
; Args (stack / cdecl):
;   [ebp+8]    x (word, sign-extended)   [ebp+0xc] y (word, sign-extended)
;   [ebp+0x10] descriptor ptr
; Registers passed to draw_sprite_rle:
;   ebx=x  ecx=y  dl=width  dh=height  esi=RLE data  edi=dest buffer  eax=0
; Globals:  0x5368  g_screen_buf (destination)
; Calls:    draw_sprite_rle @ 0x4a734
;
draw_sprite_rle_buf:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp
        mov     esi, dword ptr [ebp + 0x10]      ; esi = descriptor ptr
        movsx   ebx, word ptr [ebp + 8]          ; ebx = x
        movsx   ecx, word ptr [ebp + 0xc]        ; ecx = y
        mov     edi, dword ptr [0x5368]          ; edi = g_screen_buf (dest)
        mov     dl, byte ptr [esi + 4]           ; dl = width
        mov     dh, byte ptr [esi + 5]           ; dh = height
        mov     eax, 0                           ; eax = 0
        mov     esi, dword ptr [esi]             ; esi = RLE pixel stream
        call    0x4a734                          ; draw_sprite_rle
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
