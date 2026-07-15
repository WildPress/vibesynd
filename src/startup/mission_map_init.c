/* frameless @ 0x22858: mission/map init sequence. Builds the g_map_cols column table
   (FUN_20d18), fixes up two pointer tables (FUN_25378 x2), clears g_10ac8, session
   init when g_radar_detail==0 (FUN_12ca8), block/tile setup (FUN_20d98), palette/screen
   calls (0x498ef/0x4997e), sprintf's the map filename (fmt g_1f4 net-game, else
   fmt g_204 with the level byte g_4170[g_10b2e]) and loads it (FUN_188e8 into
   g_5378), clears + presents the screen buffer g_screen_buf (0x4993b/0x4987e), sets
   g_5324 = g_5308+6, zeroes three state bytes, inits the 3 pools (FUN_22768) and
   the 32-table (FUN_35ed8), then fills the 12-byte record at 0x1bc70 (dir-table
   velocity pair from g_dir_dx/g_dir_dy[0x10], period 0xd48, misc bytes) and clamps
   g_1be34 to >= 0x12.

   PARKED at ~96% (415/415 length, byte-identical through 0xfc): (1) ours sinks the
   `mov [g_5324],eax` store below the three dh byte-zero stores (target keeps it
   right after add eax,6; both source orders produce the same sink); (2) the tail
   record-blast's byte-const register POOL differs � target assigns cl=0x10,
   al=0x14, ah=1, ours cl/ch/al � statement reorder to the target's emission order
   didn't flip it; 4000 cpermute variants no match. Scheduler/const-pool wall,
   same family as 0x264a8's entry batching. */
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
extern int g_5324;
extern unsigned char g_10b3f, g_10b3e, g_10b50;
extern short g_dir_dx[];
extern short g_dir_dy[];
extern short g_1bc70, g_1bc72;
extern unsigned short g_1bc74;
extern unsigned char g_1bc76, g_1bc77, g_1bc78, g_1bc79, g_1bc7a, g_1bc7b;
extern short g_1be34;
extern void FUN_00020d18(int);
extern void FUN_00025378(int, int, int);
extern void FUN_00012ca8(void);
extern void FUN_00020d98(void);
extern void FUN_000498ef(int);
extern void FUN_0004997e(int);
extern void FUN_0003a4fa(char *buf, char *fmt, ...);
extern void FUN_000188e8(char *, int);
extern void FUN_0004993b(int, int);
extern void FUN_0004987e(int);
extern void FUN_00022768(void);
extern void FUN_00035ed8(void);

void mission_map_init(void)
{
    char buf[20];

    FUN_00020d18(g_535c);
    FUN_00025378(g_5314, g_5318, g_5330);
    FUN_00025378(g_531c, g_5320, g_5334);
    g_10ac8 = 0;
    if (g_radar_detail == 0)
        FUN_00012ca8();
    FUN_00020d98();
    FUN_000498ef(0x12);
    FUN_0004997e(0);
    if (g_radar_detail != 0)
        FUN_0003a4fa(buf, g_1f4);
    else
        FUN_0003a4fa(buf, g_204, g_4170[g_10b2e]);
    FUN_000188e8(buf, g_5378);
    FUN_0004993b(g_screen_buf, 0);
    FUN_0004987e(g_screen_buf);
    g_5324 = g_5308 + 6;
    g_10b3f = 0;
    g_10b3e = 0;
    g_10b50 = 0;
    FUN_00022768();
    FUN_00035ed8();
    g_1bc77 = 0x10;
    g_1bc76 = 0x14;
    g_1bc79 = 1;
    g_1bc78 = 0x10;
    g_1bc74 = 0xd48;
    g_1bc70 = g_dir_dx[0x10] * 0x10 >> 8;
    g_1bc7a = 0x28;
    g_1bc72 = g_dir_dy[0x10] * 0x10 >> 8;
    g_1bc7b = 0xa;
    if (g_1be34 < 0x12)
        g_1be34 = 0x12;
}
