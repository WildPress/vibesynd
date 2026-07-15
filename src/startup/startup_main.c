/* startup_main @ 0x24be8 (1395B) - command-line / startup parser ("main" arg loop).
 *
 * Reads the current BIOS video mode (int 0x10, AH=0x0F) via int386 helper 0x3adb2,
 * builds a command-line string (12-byte prefix from 0x355c + " "<arg> for each argv)
 * and passes it to 0x3af38, then sets a wall of default flags. It then walks argv a
 * second time dispatching single-letter options ('-'/'/' prefix, case-insensitive):
 *   ?    restore video, print usage (0x25168), exit(1)
 *   c/C  g_language = atol(arg+2); clamp >=3 -> 0
 *   d/D  g_input_echo = 1
 *   h/H  g_105 = 2
 *   i/I  sound card: -iirqN (g_4d94), -idmaN (g_4d96), -iioNNN (g_4d98 via 0x24b08)
 *   l/L  g_105 = 4
 *   n/N  strcpy(g_name_buf, arg+2)
 *   p/P  g_10b53 = atol(arg+2); clamp >0x10 -> 0x10
 *   s/S  g_sound_enabled = g_music_enabled = 0
 *   else printf(0x356c, arg+1); exit(1)
 * After the loop: init/report per g_105 (0x18338), install pool records (0x253a8),
 * bring up subsystems (0x25338/34c28), optional sound init (0x35d08 / 0x38cf8),
 * timer + a ~0x48-tick spin wait, then teardown and video-mode restore.
 *
 * Recipe: -4s -oneatx -zp8 -s -zq
 */

struct s12 { long a, b, c; };

extern void int386(int inter, void *in, void *out);   /* int386 */
extern void FUN_0003af38(void *s);
extern long FUN_0003a526(char *s);                          /* atol */
extern int  parse_hex_arg(char *s);                          /* hex parse */
extern void FUN_00025168(void);
extern void FUN_0003ad89(int code);
#pragma aux FUN_0003ad89 aborts;
extern void FUN_0003ad66(char *fmt, char *s);
extern void validate_records_or_abort(char *p);
extern void pool_records_add(unsigned char *start, unsigned char *end, int arg3);
extern void init_input_subsystem(void);
extern void noop_ret(void);
extern int  sound_driver_init(unsigned short a, unsigned short b, unsigned short c);
extern int  xmidi_music_init(int a1, int a2, unsigned short a, unsigned short b, unsigned short c);
extern void FUN_0001c168(void);
extern void guarded_init_alloc(void);
extern void main_game_loop(void);
extern void FUN_000252d8(void);
extern void FUN_00025348(void);
extern void walk_records_2c(char *p);
extern void FUN_00039188(void);
extern void FUN_00036168(void);

extern char *strcpy(char *dst, const char *src);
extern char *strcat(char *dst, const char *src);
#pragma intrinsic(strcpy)
#pragma intrinsic(strcat)

extern unsigned char  g_entry_video_mode;
extern unsigned short g_10b2e, g_1be30, g_506c;
extern unsigned char  g_105;
extern unsigned char  g_radar_detail, g_10b42, g_10b44, g_10b3b, g_10b3a, g_input_echo;
extern unsigned char  g_10b47, g_unlimited_funds, g_sound_enabled, g_music_enabled, g_539a;
extern unsigned char  g_keep_synd_colours, g_language, g_10b53, g_10b52, g_10b48;
extern volatile int   g_10ad8;
extern unsigned short g_4d94, g_4d96, g_4d98;
extern char           g_name_buf[];
extern unsigned char *g_5308, *g_530c, *g_5324;
extern int            g_5310;
extern char           g_4d9c[], g_4ea4[];
extern char           g_3584[], g_3594[];
extern char           g_355c[], g_3568[], g_356c[];

void startup_main(short argc, char **argv)
{
    char cmdbuf[0x64];
    int  buf[7];
    short i;
    unsigned char mode;
    short start;
    unsigned char c;

    ((unsigned char *)buf)[1] = 0xf;
    int386(0x10, buf, buf);
    mode = ((unsigned char *)buf)[0];
    *(struct s12 *)cmdbuf = *(struct s12 *)g_355c;
    g_entry_video_mode = mode;

    for (i = 1; i < argc; i++) {
        strcat(cmdbuf, g_3568);
        strcat(cmdbuf, argv[i]);
    }
    FUN_0003af38(cmdbuf);

    g_10b2e = 1;
    g_1be30 = 1;
    g_105 = 2;
    g_radar_detail = 0;
    g_10b42 = 0;
    g_10b44 = 0;
    g_10b3b = 0;
    g_10b3a = 1;
    g_input_echo = 0;
    g_10b47 = 1;
    g_unlimited_funds = 0;
    g_sound_enabled = 1;
    g_music_enabled = 1;
    g_539a = 0;
    g_10ad8 = 0;
    g_keep_synd_colours = 0;
    g_language = 0;
    g_10b53 = 0;
    g_10b52 = 0;
    g_10b48 = 0;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '/' || argv[i][0] == '-') {
            c = argv[i][1];
            switch (c) {
            case 'C':
            case 'c':
                g_language = (unsigned char)FUN_0003a526(argv[i] + 2);
                if (g_language >= 3)
                    g_language = 0;
                break;
            case 'D':
            case 'd':
                g_input_echo = 1;
                break;
            case 'P':
            case 'p':
                g_10b53 = (unsigned char)FUN_0003a526(argv[i] + 2);
                if (g_10b53 > 0x10)
                    g_10b53 = 0x10;
                break;
            case 'H':
            case 'h':
                g_105 = 2;
                break;
            case 'I':
            case 'i':
                if (argv[i][2] == 'i' && argv[i][3] == 'r' && argv[i][4] == 'q')
                    g_4d94 = (unsigned short)FUN_0003a526(argv[i] + 5);
                if (argv[i][2] == 'd' && argv[i][3] == 'm' && argv[i][4] == 'a')
                    g_4d96 = (unsigned short)FUN_0003a526(argv[i] + 5);
                if (argv[i][2] == 'i' && argv[i][3] == 'o')
                    g_4d98 = (unsigned short)parse_hex_arg(argv[i] + 4);
                break;
            case 'L':
            case 'l':
                g_105 = 4;
                break;
            case 'S':
            case 's':
                g_sound_enabled = 0;
                g_music_enabled = 0;
                break;
            case '?':
                *(unsigned short *)buf = mode;
                int386(0x10, buf, buf);
                FUN_00025168();
                FUN_0003ad89(1);
                break;
            case 'N':
            case 'n':
                strcpy(g_name_buf, argv[i] + 2);
                break;
            default:
                FUN_0003ad66(g_356c, argv[i] + 1);
                FUN_0003ad89(1);
            }
        }
    }

    if (g_105 & 2) {
        validate_records_or_abort(g_4d9c);
    } else if (g_105 & 4) {
        g_10b42 = 0;
        validate_records_or_abort(g_4ea4);
    }

    pool_records_add(g_5308, g_530c, g_5310);
    g_5324 = g_5308 + 6;
    g_506c = 1;
    init_input_subsystem();
    noop_ret();

    if (g_sound_enabled != 0)
        g_sound_enabled = (unsigned char)sound_driver_init(g_4d94, g_4d96, g_4d98);
    if (g_music_enabled != 0)
        g_music_enabled = (unsigned char)xmidi_music_init((int)g_3594, (int)g_3584, g_4d94, g_4d96, g_4d98);

    if (g_10b42 != 0) {
        FUN_0001c168();
    } else {
        guarded_init_alloc();
        main_game_loop();
        start = (short)g_10ad8;
        while ((unsigned int)(g_10ad8 - start) <= 0x48)
            i++;
        FUN_000252d8();
    }

    FUN_00025348();
    walk_records_2c(g_4d9c);
    FUN_00039188();
    FUN_00036168();
    noop_ret();

    ((unsigned char *)buf)[0] = mode;
    ((unsigned char *)buf)[1] = 0;
    int386(0x10, buf, buf);
}
