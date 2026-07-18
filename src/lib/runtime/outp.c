/* C runtime: outp @ 0x3b22d (CLIB3S). out dx,al -- replicated via #pragma aux
   (value passed in 8-bit AL so it loads with mov al,[..] not movzx). */
extern unsigned char __outp(unsigned port, unsigned char val);
#pragma aux __outp = "out dx,al" parm [edx] [al] value [al] modify exact [];
unsigned char outp(unsigned port, unsigned char val)
{
    return __outp(port, val);
}
