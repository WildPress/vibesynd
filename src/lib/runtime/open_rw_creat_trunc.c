/* framed forwarder @ 0x3addb: push [ebp+c]; push 0x62; push [ebp+8]; call 0x3a579 */
extern void open(int a, int mid, int b);
void open_rw_creat_trunc(int a, int b)
{
    open(a, 0x62, b);
}
