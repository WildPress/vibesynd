/* framed forwarder @ 0x3db69: push ebp; mov ebp,esp; push [ebp+8]; call 0x3e590; add esp,4; pop ebp; ret */
extern void unlink(int x);
void remove(int x)
{
    unlink(x);
}
