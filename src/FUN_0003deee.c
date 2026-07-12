/* C runtime: strnicmp @ 0x0003deee (CLIB3S). Size 77B. NEAR-MISS (parked). Same as stricmp: faithful structure, but 9.5
   binds the count n to callee-saved ebx and ours to ecx. Register-role wall (playbook S3).
   Built from Open Watcom bld/clib/string/c/strnicmp.c form (9.5 increments at loop bottom). */
int FUN_0003deee(const char *s, const char *t, unsigned n)
{
    unsigned char c1;
    unsigned char c2;

    for( ; n > 0; --n ) {
        c1 = *s;
        if( c1 >= 'A' && c1 <= 'Z' )
            c1 += 'a' - 'A';
        c2 = *t;
        if( c2 >= 'A' && c2 <= 'Z' )
            c2 += 'a' - 'A';
        if( c1 != c2 )
            return( c1 - c2 );
        if( c1 == 0 )
            break;
        ++s;
        ++t;
    }
    return( 0 );
}
