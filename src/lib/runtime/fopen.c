/* framed forwarder @ 0x3b8f8: push 0; push [ebp+c]; push [ebp+8]; call 0x3b8cd */
extern void fsopen(int a, int b, int c);
void fopen(int a, int b)
{
    fsopen(a, b, 0);
}
