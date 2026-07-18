; FUN_0004987e @ 0x4987e  (113 bytes) -- hand-written assembly (fully commented).
;
; FUN_0004987e: upload a palette to the VGA DAC. The size depends on the render mode:
;   bit 1 (planar) or bit 2 set -> load 16 colours (0x30 = 16*3 RGB bytes)
;   bit 0 set                   -> load 256 colours (0x300 = 256*3 RGB bytes)
;
; The 16-colour path first walks BIOS palette registers 0..15 (INT 0x10, AX=0x1000)
; setting register i to attribute i, i.e. an identity mapping so the 16 EGA-style
; attributes address DAC entries 0..15 directly. Both paths then reset the DAC write
; index to 0 (port 0x3c8) and stream the RGB bytes out through the DAC data port
; (0x3c9) with rep outsb.
;
; Confirmed as a palette loader by its one known caller, the message-line setter at
; 0x35638, which calls it with a palette pointer (g_10aa0).
;
; Args (stack / cdecl):
;   [ebp+8]    pointer to packed RGB palette bytes (source for outsb)
; Globals:  0x105  render-mode flags
; Ports:    0x3c8  VGA DAC write-index      0x3c9  VGA DAC data
;           INT 0x10 AH=0x10 AL=0x00        set-individual-palette-register BIOS call
;
FUN_0004987e:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        test    byte ptr [0x105], 2              ; planar mode?
        jne     pal16                            ;   yes -> 16-colour upload
        test    byte ptr [0x105], 4              ; discard-layer mode?
        je      try_pal256                       ;   no -> fall to the 256-colour test
                                                 ;   (bit 2 set also uses the 16-colour path)
; --- set BIOS palette registers 0..15 to an identity mapping ---
pal16:
        mov     bx, 0                            ; bh = register, bl = attribute
        mov     cx, 0x10                         ; 16 registers
attr_loop:
        push    bx
        push    cx
        mov     ax, 0x1000                       ; BIOS: set palette register
        int     0x10                             ;   register bh <- colour bl
        pop     cx
        pop     bx
        inc     bh                               ; next register
        inc     bl                               ; next attribute
        loop    attr_loop
; --- stream 16 DAC colours (48 bytes) ---
        mov     esi, dword ptr [ebp + 8]         ; esi = palette source
        mov     dx, 0x3c8                        ; DAC write-index port
        mov     al, 0
        out     dx, al                           ; start at DAC entry 0
        mov     dl, 0xc9                         ; dx = 0x3c9 (DAC data port)
        mov     ecx, 0x30                        ; 16 colours * 3 bytes
        rep outsb dx, byte ptr [esi]             ; write the RGB triples
        jmp     done
; --- 256-colour path ---
try_pal256:
        test    byte ptr [0x105], 1              ; 8bpp mode?
        je      done                             ;   no -> nothing to upload
        mov     esi, dword ptr [ebp + 8]         ; esi = palette source
        mov     dx, 0x3c8
        mov     al, 0
        out     dx, al                           ; start at DAC entry 0
        mov     dl, 0xc9                         ; dx = DAC data port
        mov     ecx, 0x300                       ; 256 colours * 3 bytes
        rep outsb dx, byte ptr [esi]             ; write the full palette
done:
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
