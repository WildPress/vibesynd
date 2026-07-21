/* mission_statusline @ 0x29ad8 -- mission status-line builder (jump-table
 * dispatcher). TRUE SIZE 372 (0x174, ret at 0x29c4b; manifest said 185).
 * 17-entry jump table at literal 0x1c340 (manifest 0x29a88), index = slot
 * kind word capped at 0x10: kind -> body: 1:0x29b99 2:0x29bbb 3:0x29baa
 * 5:0x29b52 0xa:0x29bbb 0xb:0x29bcc 0xe:0x29b66 0xf:0x29b77 0x10:0x29b88;
 * 0,4,6,7,8,9,0xc,0xd -> default (continue at 0x29c0d).
 *
 * Centers string arg1 into the 16-char field 0x10554 (center_string_16 =
 * center-into-16-char-field), stores arg2 word to g_537c. If byte g_radar_detail
 * set: done. If flags g_in_mission & 6: bit1 -> draw_status_labels, bit2 (re-read
 * after the call) -> draw_localized. Else scan the 8 14-byte slots at
 * 0x1be3a: first slot with dword+0 == 0 and known kind (word+4) centers
 * the per-language string tbl_44xx[g_language] into field 0x10564 and, if the
 * slot index changed (g_status_slot), stores it and resets blink counter g_status_blink
 * to 0x10. Epilogue centers string 0x3828 into field 0x10574.
 *
 * MATCHED (TRUE-SIZE MASKED MATCH at 372; match95 length check fails only
 * because the manifest size is stale). Levers that mattered:
 * - CROSS-JUMP LAW (proven by experiment battery): -oneatx merges the
 *   identical per-case call tails (push 0x10564/call/add esp) into ONE
 *   block ONLY when every arm leaves the switch via break/fall-out; any
 *   goto/continue/return arm kills the merge for ALL arms. Here the taken
 *   cases end with `goto hit` -- the trick is that case 0xb ends with
 *   plain `break` FALLING into the `hit:` code, so the 7 goto arms
 *   cross-jump into case 0xb copy (per-case push regs come out exactly:
 *   ecx/ebp/edi/esi/ebp/esi/edi/edx).
 * - switch on the loaded EXPRESSION (no named w): homes the selector in DX
 *   (mov dx,[..]; cmp dx,0x10; mov ax,dx; and eax,0xffff); a named local
 *   folds it into AX and loses the mov ax,dx widen copy.
 * - no fl local: `(g_in_mission & 6)` / `& 2` re-reads CSE into DL across the
 *   branch, and the `& 4` after the call re-reads memory (f6 05 test).
 * - store order `g_status_blink = 0x10; g_status_slot = bx;` makes Watcom stage 0x10 in
 *   ECX and tail-reorder the stores (532e first, then cx) -- the direct
 *   source order emits a 66c705 imm16 store (-3B).
 * Recipe: -4s -oneatx -zp8 -s -zq. */
extern unsigned char g_radar_detail;
extern unsigned char g_in_mission;
extern unsigned short g_537c;
extern unsigned short g_status_slot;
extern unsigned short g_status_blink;
extern unsigned char g_language;
extern unsigned char g_objectives[];
extern int tbl_4408[];
extern int tbl_4414[];
extern int tbl_4420[];
extern int tbl_442c[];
extern int tbl_4438[];
extern int tbl_4444[];
extern int tbl_4450[];
extern int tbl_445c[];
extern void center_string_16(int a, int b);
extern void draw_status_labels(void);
extern void draw_localized(void);

void mission_statusline(int a, int b)
{
    short bx;

    center_string_16(0x10554, a);
    g_537c = (unsigned short)b;
    if (g_radar_detail == 0) {
        if ((g_in_mission & 6) == 0) {
            for (bx = 0; bx < 8; bx++) {
                if (*(long *)&g_objectives[bx * 14] != 0)
                    continue;
                switch (*(unsigned short *)&g_objectives[bx * 14 + 4]) {
                case 5:
                    center_string_16(0x10564, tbl_4408[g_language]);
                    goto hit;
                case 0xe:
                    center_string_16(0x10564, tbl_4414[g_language]);
                    goto hit;
                case 0xf:
                    center_string_16(0x10564, tbl_4420[g_language]);
                    goto hit;
                case 0x10:
                    center_string_16(0x10564, tbl_442c[g_language]);
                    goto hit;
                case 1:
                    center_string_16(0x10564, tbl_4438[g_language]);
                    goto hit;
                case 3:
                    center_string_16(0x10564, tbl_4444[g_language]);
                    goto hit;
                case 2:
                case 0xa:
                    center_string_16(0x10564, tbl_4450[g_language]);
                    goto hit;
                case 0xb:
                    center_string_16(0x10564, tbl_445c[g_language]);
                    break;
                case 0:
                case 4:
                case 6:
                case 7:
                case 8:
                case 9:
                case 0xc:
                case 0xd:
                default:
                    continue;
                }
hit:
                if (g_status_slot != bx) {
                    g_status_blink = 0x10;
                    g_status_slot = bx;
                }
                break;
            }
            center_string_16(0x10574, 0x3828);
            return;
        }
        if (g_in_mission & 2)
            draw_status_labels();
        if (g_in_mission & 4)
            draw_localized();
    }
}
