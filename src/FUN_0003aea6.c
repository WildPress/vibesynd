/* C runtime: strncmp @ 0x3aea6 (CLIB3S). Size 50B. NEAR-MISS (parked).
   Open Watcom bld/clib/string/c/strncmp.c portable path (32-bit skips the 16-bit
   _fast_strncmp). Structure is faithful but 9.5's codegen shares ONE epilogue and keeps
   the compare temp in eax; ours duplicates the epilogue and routes via ebx. Register-role
   + tail-merge wall (playbook S3); not source-steerable (tried for/do-while/result-var,
   6 -o variants). OW v2 source differs subtly from the 9.5 build. */
int FUN_0003aea6(const char *s, const char *t, unsigned n)
{
    for( ; n != 0; ++s, ++t, --n ) {
        if( *s != *t )
            return( (unsigned char)*s - (unsigned char)*t );
        if( *s == 0 ) {
            break;
        }
    }
    return( 0 );
}
