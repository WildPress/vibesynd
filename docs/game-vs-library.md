# Game code vs the library

A useful thing to realise early: the program we're decompiling isn't all "the
game". A good chunk of it is code the game's authors didn't write.

## Where a program actually comes from

When you build a C program, your own code is only part of what ends up in the
final file. The compiler also bundles in its **runtime library**, a collection of
ready-made functions for common jobs: copying strings, comparing text, reading and
writing files, converting numbers, and so on. Things like `strcpy`, `tolower`, and
`fopen` that C programmers use without thinking about where they come from.

Your code and the library get combined by a program called the **linker**, which
stitches all the pieces into one executable. So the finished game is the authors'
code plus Watcom's library, stapled together.

```mermaid
flowchart TD
    G[The game's own code<br/>the authors wrote] --> L[Linker]
    R[Watcom runtime library<br/>strcpy, fopen, tolower, ...] --> L
    L --> E[One finished executable<br/>game + library, stapled together]
```

## The seam

We found that these two parts sit in different regions of the game's code. The
game's own functions are spread through the lower part, and the linked-in library
sits clustered near the top, from about address `0x3a000` onwards.

We didn't just guess this. We took the actual Watcom library files, the ones the
linker would have pulled from, and searched them for the exact bytes of each
function in that top region. The library functions' bytes are *literally present*
in the library files, which is proof they came from there rather than from the
game's source. That work is `tools/libmatch.py` and `tools/libname.py`, and the
results are named in `manifest/library_functions.md`.

## Why it matters

The seam is useful for both what we're doing now and what comes later.

For the matching decompilation, it tells us not to waste effort. There's no point
hand-writing C to recreate `strcpy`, because it isn't the game's code and we'd
never be reconstructing the authors' intent. When it comes time to build the whole
thing, we link the real library, exactly as the original did, and it drops in
identically. So the decompilation effort stays focused on the game's own logic.

For the future port, the seam is where the swap happens. On a modern system you
don't port Watcom's `strcpy`, you just use the modern compiler's own version, which
is there for free. The handful of genuinely DOS-specific bits, like the direct
hardware and file access, get replaced with modern equivalents. Everything above
the seam, the actual game, is what carries across.

## A note on hand-written assembly

A few functions in that region aren't C at all, they're hand-written assembly:
things that talk directly to the DOS operating system or to hardware ports. Those
were never going to come from a C compiler, so we don't try to match them from C
either. They're part of the runtime plumbing, not the game.
