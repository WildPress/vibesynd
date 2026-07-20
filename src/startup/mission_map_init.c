/* BEHAVIOURALLY EQUIVALENT (verified 2026-07-21): scheduler-fill tie. Sole residual is
   `xor dh,dh` hoisted into the add->store latency gap -- target emits add eax,6 / xor dh,dh /
   store eax / 3 byte stores; ours emits add eax,6 / store eax / xor dh,dh / 3 byte stores.
   Independent register-zero vs independent store reordered; identical writes (g_5324=g_5308+6,
   g_10b3f=g_10b3e=g_10b50=0), same calls. Rest of the 415B body is byte-identical.

   frameless @ 0x22858: mission/map init sequence. Builds the g_map_cols column table
   (FUN_20d18), fixes up two pointer tables (FUN_25378 x2), clears g_10ac8, session
   init when g_radar_detail==0 (FUN_12ca8), block/tile setup (FUN_20d98), palette/screen
   calls (0x498ef/0x4997e), sprintf's the map filename (fmt g_1f4 net-game, else
   fmt g_204 with the level byte g_4170[g_10b2e]) and loads it (FUN_188e8 into
   g_5378), clears + presents the screen buffer g_screen_buf (0x4993b/0x4987e), sets
   g_5324 = g_5308+6, zeroes three state bytes, inits the 3 pools (FUN_22768) and
   the 32-table (FUN_35ed8), then fills the 12-byte record at 0x1bc70 (dir-table
   velocity pair from g_dir_dx/g_dir_dy[0x10], period 0xd48, misc bytes) and clamps
   g_1be34 to >= 0x12.

   IMPROVED to dist 137 (raw; almost all residual is masked-reloc noise). Two of the
   prior walls are now SOLVED and the whole body matches target byte-for-byte except a
   single 7-byte region at 0xff:
   (1) STORE-SINK fixed: marking g_5324 `volatile` pins `mov [g_5324],eax` right after
       `add eax,6` (target order) instead of sinking it below the three dh byte stores.
   (2) CONST-POOL fixed: reordering the tail byte-blast so the two 0x10 stores are
       adjacent (g_1bc77,g_1bc78 first) and hoisting g_1bc74=0xd48 between the 0x14 and
       the 1 makes the allocator emit cl=0x10, al=0x14, edx=0xd48, ah=1 exactly like
       target (was cl/ch/al). Tail from 0x122 onward is now identical.
   Residual (first diff 0xff): target squeezes `xor dh,dh` into the add->store latency
   gap (add eax,6 / xor dh,dh / store eax / byte stores); ours emits store then xor.
   Pure scheduler-fill tie -- volatile pins the store but our codegen won't insert the
   fill; source reorders / shared-zero-local / byte-volatile don't move it. */
extern int g_535c, g_5314, g_5318, g_5330, g_531c, g_5320, g_5334;
extern int g_10ac8;
extern unsigned char g_radar_detail;
extern char g_1f4[];
extern char g_204[];
extern unsigned char g_4170[];
extern unsigned short g_10b2e;
extern int g_5378;
extern int g_screen_buf;
extern int g_5308;
extern volatile int g_5324;
extern unsigned char g_10b3f, g_10b3e, g_10b50;
extern short g_dir_dx[];
extern short g_dir_dy[];
extern short g_1bc70, g_1bc72;
extern unsigned short g_1bc74;
extern unsigned char g_1bc76, g_1bc77, g_1bc78, g_1bc79, g_1bc7a, g_1bc7b;
extern short g_1be34;
extern void relocate_map_columns(int);
extern void add_c_stride6(int, int, int);
extern void session_init(void);
extern void vehicle_hp_stamp(void);
extern void set_video_mode(int);
extern void clear_vga_screen(int);
extern void sprintf(char *buf, char *fmt, ...);
extern void load_unpack_file(char *, int);
extern void clear_offscreen(int, int);
extern void upload_palette(int);
extern void find_free_pool_slots(void);
extern void zero_even_bytes(void);

void mission_map_init(void)
{
    char buf[20];

    relocate_map_columns(g_535c);
    add_c_stride6(g_5314, g_5318, g_5330);
    add_c_stride6(g_531c, g_5320, g_5334);
    g_10ac8 = 0;
    if (g_radar_detail == 0)
        session_init();
    vehicle_hp_stamp();
    set_video_mode(0x12);
    clear_vga_screen(0);
    if (g_radar_detail != 0)
        sprintf(buf, g_1f4);
    else
        sprintf(buf, g_204, g_4170[g_10b2e]);
    load_unpack_file(buf, g_5378);
    clear_offscreen(g_screen_buf, 0);
    upload_palette(g_screen_buf);
    g_5324 = g_5308 + 6;
    g_10b3f = 0;
    g_10b3e = 0;
    g_10b50 = 0;
    find_free_pool_slots();
    zero_even_bytes();
    g_1bc77 = 0x10;
    g_1bc78 = 0x10;
    g_1bc76 = 0x14;
    g_1bc74 = 0xd48;
    g_1bc79 = 1;
    g_1bc70 = g_dir_dx[0x10] * 0x10 >> 8;
    g_1bc7a = 0x28;
    g_1bc72 = g_dir_dy[0x10] * 0x10 >> 8;
    g_1bc7b = 0xa;
    if (g_1be34 < 0x12)
        g_1be34 = 0x12;
}
