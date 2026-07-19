/* framed @ 0x3cc26: thin wrapper. Calls dos_findfirst(param_1, 7, &buf) with a
   0x2c-byte local scratch buffer, returns (result == 0) as an int (SETZ/MOVZX).

   MATCHED, recipe `-3s -os -zp8 -s -zq`. `-os` (NOT `-of`) is what emits the
   ENTER 0x2c,0/LEAVE frame the target uses; `-of` forces the longer
   push;mov;sub / mov;pop manual frame and diverges at byte 0. */
extern int dos_findfirst(int, int, void *);

int spawn_load_check(int param_1)
{
    unsigned char buf[0x2c];
    return dos_findfirst(param_1, 7, buf) == 0;
}
