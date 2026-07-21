/* frameless @ 0x28878: chunked dispatch. Call xfer_buf_req94(p1,p2,p3,chunk) for
   param_4/g_net_chunk_size whole chunks of g_net_chunk_size (16-bit global divisor), advancing p3 by
   g_net_chunk_size each time; stop early if a call returns non-zero. Then one final call
   with the remainder param_4 % g_net_chunk_size. The div is recomputed each iteration
   (the call may change g_net_chunk_size). Callee's chunk arg is 16-bit (unsigned short). */
extern unsigned short g_net_chunk_size;
extern short xfer_buf_req94(int, unsigned int, unsigned int *, unsigned short);
void xfer_chunked(int param_1, unsigned short param_2, unsigned int *param_3, unsigned int param_4)
{
    unsigned int i = 0;
    goto test;
    do {
        if (xfer_buf_req94(param_1, param_2, param_3, g_net_chunk_size))
            return;
        i++;
        param_3 = (unsigned int *)((int)param_3 + g_net_chunk_size);
    test:;
    } while (i < param_4 / g_net_chunk_size);
    xfer_buf_req94(param_1, param_2, param_3, param_4 % g_net_chunk_size);
}
