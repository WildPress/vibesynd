/* @ 0x20158: MATCHED (RELOC-AWARE) 1028B, -4s -oneatx -zp8 -s -zq.
   Levers: label strings spelled as *(char**)((char*)g_47xx + g_language*4) (the bare
   g_47xx[g_language] index freed EAX and let g_text_pal take the 5-byte A1 moffs load
   instead of the target's 6-byte 8B form); the g_4960 description slot both
   guard and body INLINE g_4960 + byte*12 (no named q pointer) so the byte read
   CSEs to the and-form widen, g_4960 materialises via MOV EBX,imm;ADD in the
   null guard, and stays a +0x4960 disp32 in the body modrm.

   item detail panel drawer. param1 selects the layout:
   ==2  -> weapon detail: blits the weapon icon (glyph (id+0x3f)*6, special 0x1d4
           for id 0x11) at (0x1f6,0x6a), then draws the name row and several
           label/value stat rows from the 0x1f5-stride record at
           g_7bf4[(id-1)*0x1f5]. Two optional stat blocks are gated by
           g_frame_enable[id]; a running y (0xdc/0xe8/+0xc) positions the shifting rows.
   !=2  -> equipment/mod detail from the 0x1eb-stride record at
           g_5780[(id-1)*0x1eb]: name + one label/value row, then (when the
           g_4960[byte*3] slot pointer is non-null) a wrapped description block
           via FUN_363d8.
   Labels come from the per-language string tables g_47a4/b0/bc/c8 indexed by
   g_language; values are sprintf'd (g_184 "%d", g_19c "%s") into buf and drawn with
   the FUN_36698 text drawer at colour 0x54 over the g_text_pal font. */
extern unsigned char *g_text_pal;
extern unsigned char g_7bf4[];
extern unsigned char g_5780[];
extern unsigned char g_language;
extern unsigned char g_frame_enable[];
extern char *g_4b10[];
extern char *g_4a38[];
extern char *g_47a4[];
extern char *g_47b0[];
extern char *g_47bc[];
extern char *g_47c8[];
extern char *g_4960[];
extern char g_184[];
extern char g_19c[];

extern void FUN_0004a6c8(int x, int y, unsigned char *spr);
extern void FUN_0003a4fa(char *buf, char *fmt, ...);
extern void FUN_00036698(char *s, unsigned short x, int a3, int a4, unsigned short a5,
                         unsigned char *tbl, signed char a7, signed char a8, int a9,
                         int a10);
extern char FUN_000363d8(char *s, int x0, int y0, int w, int h,
                         unsigned char *font, unsigned short a7, unsigned char a8,
                         signed char a9, int a10, char **out);

void FUN_00020158(unsigned char param1, unsigned char param2)
{
    char buf[40];
    unsigned short y;

    if (param1 == 2) {
        if (param2 == 0x11)
            FUN_0004a6c8(0x1f6, 0x6a, g_text_pal + 0x1d4);
        else
            FUN_0004a6c8(0x1f6, 0x6a, g_text_pal + (param2 + 0x3f) * 6);

        FUN_0003a4fa(buf, g_19c,
                     *(char **)((char *)g_4b10 + g_language * 4
                                + g_7bf4[(param2 - 1) * 0x1f5] * 12));
        FUN_00036698(buf, 0x1f8, 0xc4, 0xe, 0x54, g_text_pal, -2, 6, 0, 0);

        FUN_00036698(*(char **)((char *)g_47a4 + g_language * 4), 0x1f8, 0xd0, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
        FUN_0003a4fa(buf, g_184, *(int *)(g_7bf4 + (param2 - 1) * 0x1f5 + 1));
        FUN_00036698(buf, 0x22c, 0xd0, 0xe, 0x54, g_text_pal, -2, 6, 0, 0);

        y = 0xdc;
        if (g_frame_enable[param2] != 0) {
            FUN_00036698(*(char **)((char *)g_47b0 + g_language * 4), 0x1f8, 0xdc, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
            FUN_0003a4fa(buf, g_184, *(short *)(g_7bf4 + (param2 - 1) * 0x1f5 + 5) + 1);
            FUN_00036698(buf, 0x22c, 0xdc, 0xe, 0x54, g_text_pal, -2, 6, 0, 0);
            y = 0xe8;
        }

        FUN_00036698(*(char **)((char *)g_47bc + g_language * 4), 0x1f8, y, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
        FUN_0003a4fa(buf, g_184, *(short *)(g_7bf4 + (param2 - 1) * 0x1f5 + 0xb));
        FUN_00036698(buf, 0x22c, y, 0xe, 0x54, g_text_pal, -2, 6, 0, 0);
        y += 0xc;

        if (g_frame_enable[param2] != 0) {
            FUN_00036698(*(char **)((char *)g_47c8 + g_language * 4), 0x1f8, y, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
            FUN_0003a4fa(buf, g_184, *(int *)(g_7bf4 + (param2 - 1) * 0x1f5 + 7));
            FUN_00036698(buf, 0x22c, y, 0xe, 0x54, g_text_pal, -2, 6, 0, 0);
        }
    } else {
        FUN_00036698(*(char **)((char *)g_4a38 + g_language * 4
                                 + g_5780[(param2 - 1) * 0x1eb] * 12),
                     0x1f8, 0x6e, 0xe, 0x54, g_text_pal, -2, 6, 0, 0);

        FUN_00036698(*(char **)((char *)g_47a4 + g_language * 4), 0x1f8, 0x7e, 0xf, 0x54, g_text_pal, -2, 8, 0, 0);
        FUN_0003a4fa(buf, g_184, *(int *)(g_5780 + (param2 - 1) * 0x1eb + 1));
        FUN_00036698(buf, 0x22c, 0x7e, 0xe, 0x54, g_text_pal, -2, 6, 0, 0);

        if ((char *)g_4960 + g_5780[(param2 - 1) * 0x1eb + 5] * 12 != 0) {
            FUN_000363d8(*(char **)((char *)g_4960
                                    + g_5780[(param2 - 1) * 0x1eb + 5] * 12
                                    + g_language * 4),
                         0x1f8, 0x8e, 0x78, 0xea, g_text_pal, 0x54, 0xc, -2, 6, 0);
        }
    }
}
