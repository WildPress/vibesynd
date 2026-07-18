; flic_play @ 0x39ca0  (418 bytes) -- hand-written assembly (fully commented).
;
; flic_play: play an Autodesk FLIC/FLC animation. Despite living in the sound module,
; this is the animation player (its helpers key off the FLC magic 0xAF12 and frame magic
; 0xF1FA, and the standard FLIC chunk types). It opens the file, walks the chunk stream,
; decodes each frame into the offscreen buffer, blits it to the VGA screen, optionally
; updates the palette, and paces/aborts playback. It can replay and can be interrupted
; by a keypress when the caller marks the animation skippable.
;
; Args (cdecl):
;   [ebp+8]   = skippable flag (nonzero -> a keypress may abort playback)
;   [ebp+0xc] = mode flag; stored in 0xbdf4 and consulted by the palette step
;   [ebp+0x10]= per-frame callback context, passed to FUN_000391a8 every frame
;
; Key globals:
;   0xbdf2  keep-playing / replay counter (word; 0 = stop)
;   0x537e  input/abort state (0x80 = idle; changing it requests an abort)  [inferred]
;   0xbdf4  mode flag (from arg 2)
;   0xbdd0  g_flic_handle    open file handle
;   0xbddc  current chunk size (dword)     0xbdd4  current chunk type (word)
;   0xbdd6  frames remaining (word)        0xbde4  frames played (word)
;   0xbdec  centisecond timestamp seed for pacing  [inferred]
;   0xbdf8  "present the frame" flag (blit/palette only when nonzero)
;   0xbe2e  palette-dirty flag (a COLOR chunk sets it to 1)
;   0x5368  g_screen_buf     decoded frame, blitted to VGA 0xa0000 (320x200 = 0x3e80 dwords)
;   0x10b3f abort-allowed flag  [inferred]
; Calls: FUN_0003a598 (open file), FUN_0003a7c4 (read bytes), flic_parse_header (parse FLC
;   header), FUN_000391a8 (per-frame callback), flic_decode_frame (decode frame), flic_load_palette
;   (apply palette), FUN_0003a19a (post-frame present/pace hook), FUN_0003a89d (close file).
; BIOS/DOS: INT 21h AH=2Ch (get time), INT 10h AH=12h BL=36h (video refresh enable).

flic_play:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        mov     word ptr [0xbdf2], 1         ; keep-playing = 1 (one pass)
        mov     byte ptr [0x537e], 0x80      ; input/abort state = idle
        mov     word ptr [0x5398], 0         ; clear state word
        mov     eax, dword ptr [ebp + 0xc]   ; arg 2 = mode flag
        mov     dword ptr [0xbdf4], eax       ;   -> 0xbdf4
play_pass:
        push    0x40                         ; open the FLIC file (name/params in 0xbdfc buffer)
        push    0x200
        push    0xbdfc
        call    0x3a598                      ; -> FUN_0003a598: returns a file handle
        add     esp, 0xc
        cmp     eax, 0                        ; open failed?
        jle     done                         ;   yes -> give up
        mov     dword ptr [0xbdd0], eax       ; g_flic_handle = handle
        mov     word ptr [0xbde4], 0         ; frames played = 0
        mov     word ptr [0xbdee], 0         ; clear pacing counter
        mov     ah, 0x2c                     ; INT 21h AH=2Ch: get system time
        int     0x21                         ;   CH:CL=h:m  DH=sec  DL=centisec
        mov     ax, 0x64                     ; 100
        mul     dh                           ; ax = seconds * 100
        mov     dh, 0
        add     dx, ax                       ; dx = sec*100 + centiseconds
        mov     word ptr [0xbdec], dx         ; pacing timestamp seed
read_chunk:
        mov     eax, 4                        ; read the 4-byte chunk size...
        push    eax
        push    0xbddc                        ;   into 0xbddc
        push    dword ptr [0xbdd0]
        call    0x3a7c4                      ; -> FUN_0003a7c4: read bytes
        add     esp, 0xc
        push    2                            ; read the 2-byte chunk type...
        push    0xbdd4                        ;   into 0xbdd4
        push    dword ptr [0xbdd0]
        call    0x3a7c4                      ; -> FUN_0003a7c4
        add     esp, 0xc
        mov     ax, word ptr [0xbdd4]        ; ax = chunk type
        cmp     ax, 0xaf12                   ; FLC file header?
        jne     chk_frame
        call    0x39ee2                      ; -> flic_parse_header: parse header (frames, w, h)
        jmp     read_chunk                   ; then read the next chunk
chk_frame:
        cmp     ax, 0xf1fa                   ; FLIC frame chunk?
        jne     close_file                   ;   neither header nor frame -> stop
        mov     ax, word ptr [0xbdd6]        ; frames remaining--
        sub     ax, 1
        mov     word ptr [0xbdd6], ax
        mov     ax, word ptr [0xbde4]        ; frames played++
        add     ax, 1
        mov     word ptr [0xbde4], ax
        push    dword ptr [ebp + 0x10]       ; per-frame callback context
        call    0x391a8                      ; -> FUN_000391a8: per-frame callback
        add     esp, 4
        call    0x39e42                      ; -> flic_decode_frame: decode this frame's sub-chunks
        cmp     dword ptr [0xbdf8], 0        ; should we present the frame?
        je      present_done                 ;   no -> skip blit/palette
        cmp     word ptr [0xbe2e], 1         ; palette dirty?
        jne     blit_frame                   ;   no -> just blit
        call    0x39f92                      ; -> flic_load_palette: apply new palette
blit_frame:
        push    ecx
        push    edi
        push    esi
        mov     edi, 0xa0000                 ; VGA framebuffer
        mov     esi, dword ptr [0x5368]      ; source = g_screen_buf (decoded frame)
        mov     ecx, 0x3e80                  ; 0x3e80 dwords = 64000 bytes (320x200)
        rep movsd dword ptr es:[edi], dword ptr [esi] ; blit to screen
        pop     esi
        pop     edi
        pop     ecx
present_done:
        call    0x3a19a                      ; -> FUN_0003a19a: post-frame present / pacing
        mov     ax, word ptr [0xbdd6]        ; frames remaining?
        cmp     ax, 0
        jne     check_continue
        jmp     close_file                   ;   last frame -> stop
check_continue:
        mov     bl, 0x36                     ; BIOS video refresh control...
        mov     ax, 0x1200                   ; ...AH=12h BL=36h AL=0 = enable refresh
        int     0x10
        cmp     dword ptr [ebp + 8], 0       ; is this animation skippable?
        je      test_continue                ;   no -> ignore input
        cmp     byte ptr [0x10b3f], 0        ; abort allowed?
        jne     stop_playing                 ;   flag set -> abort
        cmp     byte ptr [0x537e], 0x80      ; input state changed from idle?
        jg      stop_playing                 ;   keypress -> abort
        jmp     test_continue
stop_playing:
        mov     word ptr [0xbdf2], 0         ; keep-playing = 0 (stop after this pass)
test_continue:
        cmp     word ptr [0xbdf2], 0         ; still playing?
        je      close_file                   ;   no -> close
        jmp     read_chunk                   ;   yes -> next chunk
close_file:
        push    dword ptr [0xbdd0]
        call    0x3a89d                      ; -> FUN_0003a89d: close the file
        add     esp, 4
        mov     bx, 0x1f4                    ; short settle delay: 500 iterations
delay_spin:
        cmp     word ptr [0xbdf2], 0         ; stopped?
        je      done                         ;   yes -> finish
        dec     bx
        jne     delay_spin
        dec     word ptr [0xbdf2]            ; one replay consumed
        jne     play_pass                    ;   more replays -> reopen and play again
done:
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret
