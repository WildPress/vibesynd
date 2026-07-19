/* @ 0x363d8 (523 bytes): word-wrap text layout/draw engine.
   Draws string s glyph-by-glyph with `font` (6-byte glyph records: width at +4,
   height at +5), word-wrapping inside the box (x0,y0,w,h). a7 = glyph base offset
   added to each char code, a8 = line height, a9 = signed char spacing, a10 unused,
   out = (optional) receives the remaining-string pointer on vertical overflow.
   Words are measured with measure_text_width; each glyph is drawn at
   (x, y+12-glyphheight) via draw_sprite_rle_buf. Separators: space, 0x0a, 0x5c, 0x7c;
   double-0x0a = blank line (y += 2*a8, x=x0), 0x5c = newline, 0x7c = terminator,
   TAB is rewritten to a space in the buffer. Returns 0 on overflow (out set), 1
   otherwise.

   PARKED near-miss 517B vs 523B target (NOT matched), EDIT-DIST 108 (was 109),
   instruction-for-instruction structural 1:1 (176 vs 177 instrs). KEY WINS kept in
   this file (each was load-bearing, found over 16 compiles):
   - x,y MUST be `unsigned short` locals (not int): this activates Watcom's
     demanded-bits narrowing -> dword slot loads/stores of 16-bit locals, the
     half-clear widens (`mov al,[a8]; xor ah,ah` with garbage upper16 tolerated),
     the `xor ah,dh` known-equal trick in the wrap block, and DERANKS x/y so the
     fp/op named copies win the callee-saved regs (edi/ebp) - int x,y always
     registerize into esi/edi instead (wrong). ushort x,y + int box params with
     (unsigned short) casts at the guards reproduced guard1-rhs + wrap byte-exact.
   - fp/op MUST be named local copies of font/out (register-resident param copy
     lever); box params x0..h must stay int (ushort params flip the rhs load order).
   - decl order y-then-x fixes the slot layout (y=[esp+4], x=[esp+8], y2=[esp]).
   - INIT order must ALSO be y=y0 THEN x=x0 (not x-then-y): emits the y-store
     (slot+4) before the x-store (slot+8) matching target's entry, -1 edit-dist
     (109->108). Swapping the guard1/guard2 add operand order still regresses.
   - draw_sprite_rle_buf declared with ushort x,y params (produces the mov ax,dx;
     and eax,0xffff arg push).
   - if/else both-arms-r=1 tail reproduces the DEAD `test ebp,ebp` (jcc-to-next
     deleted by cross-jumping, test survives). Ternary op?1:1 also works but can
     mis-home the test to a stack slot.
   REMAINING diffs - all register-role/scheduler family (park class):
   (a) entry: ours interleaves x/y init stores before the edi/ebp loads and
       splits `cmp [ebx],0` into `mov ah,[ebx]; test ah,ah` (0x264a8-family
       entry-scheduler wall);
   (b) guard1 lhs: ours `mov cx,[x]; add ecx,eax` vs target `mov dx,[x];
       lea ecx,[edx+eax]` (fresh-reg 3-op add; named temps/operand swaps inert);
   (c) guard2 mirror: identical loads then `add edx,eax` vs `add eax,edx`,
       cascading into y-read reg (eax vs edx), y2 in-place `add eax,0xc` (83-form)
       vs copy `mov eax,edx; add eax,0xc` (05 imm32-form!), and the cl-load
       hoisting above the y2 store (breaks the inner-loop *s CSE -> al reloads);
   (d) glyph-loop roles: c in edx vs ecx, second index in-place `add edx,esi` vs
       `lea edx,[esi+ecx]`, movsx dx vs ax for a9, x-load ecx/esi swapped between
       inner-width and space-width blocks, separator const pool 0x5c/0x7c homes
       swapped (cl vs dh);
   (e) tail r homed in AL (mov al,1 / xor al,al) vs target BL (mov bl,1;
       mov al,bl / xor bl,bl).
   Good permuter candidate: one add-direction flip in guard2 likely cascades
   through (c) and (d). */

extern int measure_text_width(char *s, unsigned char *font, int base, int spacing);
extern void draw_sprite_rle_buf(unsigned short x, unsigned short y, unsigned char *glyph);

char draw_wrapped_text(char *s, int x0, int y0, int w, int h,
                  unsigned char *font, unsigned short a7, unsigned char a8,
                  signed char a9, int a10, char **out)
{
    unsigned short y;
    unsigned short x;
    int y2;
    int ww;
    char r;
    unsigned char *fp = font;
    char **op = out;

    y = y0;
    x = x0;
    while (*s != 0 && *s != 0x7c) {
        ww = measure_text_width(s, fp, a7, a9);
        if ((unsigned short)ww + x > (unsigned short)x0 + (unsigned short)w) {
            x = (unsigned short)x0;
            y += a8;
        }
        if (y > (unsigned short)y0 + (unsigned short)h) {
            if (op) {
                *op = s;
            }
            r = 0;
            goto done;
        }
        y2 = y + 0xc;
        while (*s != 0 && *s != 0x20 && *s != 0xa && *s != 92 && *s != 0x7c) {
            if (*s == 9) {
                *s = 0x20;
            }
            draw_sprite_rle_buf(x, y2 - *(fp + (a7 + *s - 0x20) * 6 + 5),
                         fp + (*s - 0x20 + a7) * 6);
            x += fp[(a7 + *s - 0x20) * 6 + 4] + a9;
            s++;
        }
        x += fp[a7 * 6 + 4];
        do {
            if (*s != 0x20 && *s != 0xa && *s != 92 && *s != 0x7c) {
                break;
            }
            if (*s == 0xa && *s == s[1]) {
                y += a8 * 2;
                x = x0;
            }
            if (*s == 92) {
                y += a8;
                x = x0;
            }
            if (*s == 0x7c) {
                break;
            }
            s++;
        } while (*s != 0);
    }
    if (op) {
        r = 1;
    } else {
        r = 1;
    }
done:
    return r;
}
