/* NEAR-MISS @ 0x2bbe8 -- 160 vs 159 code bytes; JUMP TABLE FULLY RECOVERED via
 * tools/lefix.py (the payoff of the fixed parser). 5-entry dispatcher at
 * obj1:+0x1e480: word +0 -> case 1/2/3 add 1/2/3, cases 0/4 + default add 0.
 * (Manifest size was undercounted 140; corrected to 159 = through the ret.)
 *
 * Animation-frame ticker over an 18-byte-record list at p, ending at a record
 * whose word +0 == -99 (0xff9d). Per active record (delay byte +0xa != 0):
 * decrement delay, add the per-type step to base frame 0xd4+word[+0x10]; if the
 * frame changed, look up sprite g_frame_sprite[frame] into word +0xc and store frame to
 * +0xe; draw via 0x1b798(sprite, x=+2, y=+4).
 *
 * cont.21 RETRY DATA (7 compiles): the back-half rotation is a demanded-bits
 * vs fold-direction either/or, plus three independent 1-2 byte diffs:
 * - `unsigned int a = sw + base;` + `(ushort)a` uses (full-width-temp lever)
 *   DOES flip the sum fold to EAX and fixes the ENTIRE downstream rotation
 *   (base=EDX `bad4000000/66035310`, `01d0` add eax,edx, idx=EDX, ptr=ECX
 *   `8b0d`, stores `6689530c/6689430e` all == target) -- BUT the int temp
 *   demands 32 bits, so Watcom materialises the ushort base widen
 *   `81e2ffff0000` (target has none: it exploits mov edx,imm32 known-zero
 *   upper16), AND old demotes ESI->CX dropping push esi. ushort `a` (this
 *   file) avoids the widen (demanded-16) but folds into base's ECX instead.
 *   sw-as-destination `sw = (ushort)(sw+base)` gives base=EDX + 16-bit add +
 *   eax widen. Split `a = sw; a += base;` regresses (3rd push). Commute and
 *   inline-base are byte-inert.
 * - old-in-ESI comes ONLY from the top-of-block decl-init (crosses the
 *   dispatch); any later assignment homes it in CX. Target has the SAME
 *   `668b730e` bytes but scheduled late (after `add dx,[ebx+0x10]`) -- ours
 *   always schedules it right after the dec. Not source-reachable so far.
 * - entry guard: target DUPLICATES the 3-byte epilogue (`7503 5e5bc3`); ours
 *   always far-jz to the shared one (`0f84 8b000000`). `if(==-99) return;`
 *   and `if(!=-99) goto body; return; body:` both still merge.
 * - loop tail: target `83c312 0fbf03` (add then load); ours folds the load
 *   pre-add `0fbf4312 83c312`. volatile on the while deref is inert.
 * Same class as 0x2d5b8 / 0x28ec8. Recipe: -4s -oneatx -zp8 -s -zq
 */
extern unsigned short *g_frame_sprite;
extern void draw_object_sprite_list(unsigned short spr, short x, short y);

void anim_tick_frame_list(unsigned char *p)
{
    if (*(short *)p != -99) {
        do {
            if (p[0xa] != 0) {
                unsigned short old = *(unsigned short *)(p + 0xe);
                unsigned short base;
                unsigned short a;
                int sw = 0;

                p[0xa]--;
                switch (*(unsigned short *)p) {
                case 0: break;
                case 1: sw = 1; break;
                case 2: sw = 2; break;
                case 3: sw = 3; break;
                case 4: break;
                }
                base = (unsigned short)(0xd4 + *(unsigned short *)(p + 0x10));
                a = (unsigned short)(sw + base);
                if (a != old) {
                    *(unsigned short *)(p + 0xc) = g_frame_sprite[a];
                    *(unsigned short *)(p + 0xe) = a;
                }
                draw_object_sprite_list(*(unsigned short *)(p + 0xc),
                             *(short *)(p + 2), *(short *)(p + 4));
            }
            p += 0x12;
        } while (*(short *)p != -99);
    }
}
