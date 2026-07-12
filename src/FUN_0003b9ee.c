/* C runtime helper @ 0x3b9ee (CLIB3S): nibble -> ASCII hex digit.
   x+=0x30; if(x>'9') x+=0x27;  (mislabeled 'fclose' by the window heuristic) */
int FUN_0003b9ee(int x)
{
    x += 0x30;
    if (x > 0x39)
        x += 0x27;
    return x;
}
