/* Toolchain smoke test: a trivial leaf function. Proves wcc386 compiles and
 * wdis disassembles inside the container. Delete once the real loop is proven. */
int sum3(int a, int b, int c)
{
    return a + b + c;
}
