; flic_load_palette @ 0x39f92  (161 bytes) -- hand-written assembly (fully commented).
;
; flic_load_palette: rebuild the 256-colour VGA palette from a run-length palette script and
; blast it to the DAC during vertical retrace. Part of the FLIC player (flic_play);
; it is called once a frame's COLOR chunk has flagged the palette dirty (0xbe2e).
;
; The script at g_10ab0 is: [word run-count] then, per run, [byte skip][byte count]
; [count*3 RGB bytes]. Each run advances the DAC-buffer cursor past `skip` entries
; (3 bytes each) and then writes `count` entries (count==0 means 256). The rebuilt
; 768-byte palette lives at g_10aa8; it is then written to DAC index 0 upward via ports
; 0x3c8/0x3c9, after disabling video refresh and waiting for the retrace so it doesn't
; snow.
;
; No args. Only acts when mode flag 0xbdf4 == 1.
; Globals: 0xbe2e palette-dirty flag (cleared on entry); 0x10ab0 g_10ab0 palette script;
;          0x10aa8 g_10aa8 768-byte DAC buffer.
; Ports:   0x3c8 DAC write-index; 0x3c9 DAC data; 0x3da Input Status #1 (retrace).
; BIOS:    INT 10h AH=12h BL=36h AL=1 -> disable video refresh during the DAC write.

flic_load_palette:
        push    esi
        mov     word ptr [0xbe2e], 0         ; clear palette-dirty flag
        cmp     dword ptr [0xbdf4], 1        ; player in mode 1?
        jne     done                         ;   no -> nothing to do
        mov     edi, dword ptr [0x10ab0]     ; edi -> palette script (g_10ab0)
        sub     bx, bx                        ; bx = 0 (unused scratch)
        mov     ax, word ptr [edi]           ; ax = number of runs
        add     edi, 2
        mov     esi, dword ptr [0x10aa8]     ; esi -> DAC buffer (g_10aa8)
next_run:
        push    ax                           ; save runs-remaining
        mov     ecx, 0
        mov     cl, byte ptr [edi]           ; cl = skip count (entries to leave)
        inc     edi
        add     esi, ecx                     ; esi += skip*3 (RGB = 3 bytes/entry)
        add     esi, ecx
        add     esi, ecx
        mov     dx, 0
        mov     dl, byte ptr [edi]           ; dl = copy count
        inc     edi
        cmp     dl, 0                        ; 0 encodes 256
        jne     copy_entry
        mov     dx, 0x100                    ; count = 256
copy_entry:
        mov     al, byte ptr [edi]           ; R
        inc     edi
        mov     byte ptr [esi], al
        inc     esi
        mov     al, byte ptr [edi]           ; G
        inc     edi
        mov     byte ptr [esi], al
        inc     esi
        mov     al, byte ptr [edi]           ; B
        inc     edi
        mov     byte ptr [esi], al
        inc     esi
        dec     dx                           ; one entry done
        cmp     dx, 0
        jne     copy_entry                   ; more entries in this run
        pop     ax                           ; restore runs-remaining
        dec     ax
        cmp     ax, 0
        jne     next_run                     ; more runs
        mov     esi, dword ptr [0x10aa8]     ; esi -> rebuilt 768-byte palette
        mov     edi, esi
        mov     cx, 0x300                    ; 768 bytes to write
        mov     dx, 0x3c8                    ; DAC write-index port
        mov     ax, 0
        out     dx, al                       ; start at colour index 0
        cld
        mov     bl, 0x36                     ; BIOS: video refresh control...
        mov     ax, 0x1201                   ; ...AH=12h BL=36h AL=1 = disable refresh
        int     0x10
        mov     dx, 0x3da                    ; Input Status #1
wait_retrace:
        in      al, dx
        test    al, 8                        ; in vertical retrace (bit 3)?
        je      wait_retrace                 ;   wait for it
        mov     dx, 0x3c9                    ; DAC data port
        rep outsb dx, byte ptr [esi]         ; write all 768 palette bytes
        mov     edi, esi                     ; leave edi at the buffer base
done:
        pop     esi
        ret
