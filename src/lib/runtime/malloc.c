/* framed 1-arg forwarder @ 0x3aa74: push [ebp+8]; call 0x3aa84 */
extern void nmalloc(int x);
void malloc(int x)
{
    nmalloc(x);
}
