/* framed 1-arg forwarder @ 0x3ab59: push [ebp+8]; call 0x3ab69 */
extern void nfree(int x);
void free(int x)
{
    nfree(x);
}
