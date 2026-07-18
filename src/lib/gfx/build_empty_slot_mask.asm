; build_empty_slot_mask @ 0004d069  (75 bytes) -- hand-written assembly (fully commented).
;
; build_empty_slot_mask: precompute a 256-byte flag table at 0xe39c from the
; type->draw-data table g_5360 (0x5360). For each of 256 entries it packs six flag
; bits (bit 0..5). Each bit corresponds to one dword offset read consecutively from
; the front of g_5360; that offset is added to the g_5360 base to reach a block of 16
; records of 0x14 (20) bytes. The routine scans the leading dword of each of the 16
; records: if all sixteen are zero (the block is empty) the flag bit is set, otherwise
; it is left clear. So the table records, per entry and per sub-field, "this draw-data
; slot is empty", presumably to let the draw code skip empty slots quickly later.
;
; This is one-time setup, not per-frame. Args: none. Clobbers eax/ebx/ecx/edx/esi/edi.
; Globals: 0x5360 g_5360 (type->draw-data table, base and offset source),
;          0xe39c output flag table (256 bytes). The build uses build_empty_slot_mask.c
; (db-transcription); this is the readable companion.
;
build_empty_slot_mask:
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        mov     esi, dword ptr [0x5360]          ; 8b3560530000  -- esi = g_5360: reads offsets from its front
        mov     edi, 0xe39c                      ; bf9ce30000 -- edi = output flag table
        mov     dx, 0xff                         ; 66baff00   -- outer counter: 256 entries (0xff..0)
outer_loop:                                      ;            <- (0x4d07e)
        mov     byte ptr [edi], 0                ; c60700     -- clear this entry's flag byte
        mov     cl, 0                            ; b100       -- cl = sub-field / bit index (0..5)
field_loop:                                      ;            <- (0x4d083)
        mov     eax, dword ptr [0x5360]          ; a160530000 -- eax = g_5360 base
        add     eax, dword ptr [esi]             ; 0306       -- + next offset -> record block
        add     esi, 4                           ; 83c604     -- advance to the following offset
        mov     ch, 0xf                          ; b50f       -- inner counter: 16 records (0xf..0)
scan_loop:                                       ;            <- (0x4d08f)
        cmp     dword ptr [eax], 0               ; 833800     -- record's leading dword zero?
        jne     0x4d0a1                          ; 750d       -- non-zero -> block not empty, leave bit clear (-> next_field)
        add     eax, 0x14                        ; 83c014     -- next record (+20 bytes)
        dec     ch                               ; fecd
        jge     0x4d08f                          ; 7df4       -- more records -> scan_loop
;       all 16 records were zero -> block is empty, set the flag bit:
        mov     bl, 1                            ; b301
        shl     bl, cl                           ; d2e3       -- bl = 1 << sub-field index
        or      byte ptr [edi], bl               ; 081f       -- set this entry's flag bit
next_field:                                      ;            <- (0x4d0a1)
        inc     cl                               ; fec1
        cmp     cl, 6                            ; 80f906
        jne     0x4d083                          ; 75db       -- 6 sub-fields per entry -> field_loop
        inc     edi                              ; 47         -- next output byte
        dec     dx                               ; 664a
        jge     0x4d07e                          ; 7dd1       -- 256 entries -> outer_loop
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        ret                                      ; c3
