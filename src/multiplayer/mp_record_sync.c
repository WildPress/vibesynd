/* frameless @ 0x272b8: multiplayer player-record sync barrier. Logs g_368c; only
   acts in multiplayer (g_num_players > 1) and when the caller id equals the local player
   id g_cur_player. For each slot i: if i is the local player, SEND our 0x417-byte record
   g_player_recs[i] to every other active player j (g_df30[j] != 0, j != self) via 0x28878
   on channel g_10644[j]/{dword,word}, returning -1 on any send error; otherwise if
   slot i is active (g_df30[i] == 1), busy-RECEIVE player i's record into g_player_recs[i]
   via 0x288f8 until a full 0x417 bytes arrive. Returns 0.
   MATCHED by cpermute (agent hand-parked at 361/363 on an EBX-vs-EAX home for the
   param load + self-compare; the fuzzer found the flipping spelling). */
extern void printf(char *fmt, ...);
extern short xfer_chunked(int a, unsigned short b, void *buf, int len);
extern int recv_chunked(int a, unsigned short b, void *buf, int len);
extern short g_num_players;
extern short g_cur_player;
extern unsigned char g_df30[];
extern char g_player_recs[][0x417];
#pragma pack(1)
struct chan
{
  int a;
  unsigned short b;
};
extern struct chan g_10644[];
extern char g_368c[];
extern char g_36a0[];
extern char g_36ac[];
int mp_record_sync(int param_1)
{
  unsigned short i;
  unsigned short j;
  printf(g_368c);
  if ((g_num_players > 1) && (g_cur_player == ((short) param_1)))
  {
    for (i = 0; i < g_num_players; i++)
    {
      if (i == g_cur_player)
      {
        for (j = 0; j < g_num_players; j++)
        {
          if ((g_df30[j] != 0) && (j != g_cur_player))
          {
            printf(g_36a0);
            if (0 != xfer_chunked(g_10644[j].a, g_10644[j].b, g_player_recs[i], 0x417))
              return -1;
          }
        }

      }
      else
        if (1 == g_df30[i])
      {
        printf(g_36ac);
        while (recv_chunked(g_10644[i].a, g_10644[i].b, g_player_recs[i], 0x417) != 0x417)
        {
        }

      }
    }

  }
  return 0;
}

