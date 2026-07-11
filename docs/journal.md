# Reverse-engineering journal

A running log of the actual work: what we took on, why, what went wrong, and how we
got there. It's the narrative version of the terse session log in `AGENTS.md`,
written so the thinking is easy to follow later. It reads in order, oldest first,
and each entry tries to show the *process*, not just the result.

If a term is unfamiliar, the [foundations pages](README.md) explain the building
blocks.

---

## The story so far (up to 51 of 500 matched)

The setup is three things working together. **Ghidra** lets us explore the original
binary and see each function's disassembly. A preserved copy of the period
[Watcom compiler](watcom.md) compiles our C the same way the game was built. And a
byte-diff, our "oracle", tells us whether our compiled function is identical to the
original. The loop is: read a function, write C, compile, diff, repeat until it
matches.

The first matches were tiny accessor functions, the kind that just return a global
or hand an argument straight on. They matched almost immediately and proved the loop
worked end to end.

Then we hit the first wall. A whole class of functions with a
[stack frame](stack-frames.md) refused to match, and the notes had written them off
as impossible. Digging in, the cause was embarrassingly small: we'd been asking the
compiler for a frame with the flag `-of+`, which adds one extra instruction the
original doesn't have. Asking more gently, `-of`, produced the exact frame. One
character in a flag unblocked dozens of functions. The lesson stuck: when something
won't match, the difference is real and specific, and finding it is the whole job.

With frames sorted, the frameless game functions started matching cleanly with the
plain stack-calling settings. But another group in the *top* region of the code kept
resisting, no matter what we tried. The penny dropped when we searched Watcom's
runtime library files for their exact bytes and found them there. Those functions
aren't the game's code at all, they're the compiler's [runtime
library](game-vs-library.md), `strcpy` and friends, stapled in by the linker. So we
stopped trying to decompile them from C and simply identified and set them aside.

The functions that remained hard were a lesson in [register
allocation](register-allocation.md). When several values are live at once, the
compiler chooses which register holds which, and if our C differs even slightly it
chooses differently, and the bytes diverge. We learned to target functions where the
choice is forced, so the compiler has no room to disagree, and to treat the free
ones as a slow tail.

Two bits of tooling then paid off. We built a regression baseline, every match with
the flags it needs, and used it to prove the [compiler flags](compiler-flags.md):
the optimisation setting is pinned, because every alternative breaks something we've
already matched. And we replaced a shortcut in the diff. It used to guess that
[relocations](relocations-and-omf.md) were four zero bytes, which missed the ones
carrying a small offset, like an array bound. Now it reads the object file's real
fixup records, so it masks exactly the right spots. That unblocked functions that use
arrays and tables, which is a lot of real game logic.

That's where we are: 51 of 500 functions matched, a bit over 1% of the game by bytes,
with the toolchain understood and the diff trustworthy. Byte coverage is low because
early matches are small, and it's the honest number to watch. From here the entries
below track the work function by function.

---

## FUN_00039f49 — a "clean" function that taught us about CPU levels

I picked this one because it looked easy: a linear call, nothing held across
anything, exactly the [forced-allocation](register-allocation.md) shape that usually
matches first try. Its disassembly:

```
movzx eax, word ptr [0xbddc]   ; read a 16-bit global, zero-extended
sub   eax, 6                    ; subtract 6
push  eax
push  dword ptr [0x5370]        ; push a global straight from memory
push  dword ptr [0xbdd0]        ; and another
call  0x0003a7c4
add   esp, 0xc                  ; three arguments, so clean up 12 bytes
ret
```

So it just calls another function with three arguments: two globals, and a third
global minus six. Reading the argument order off the pushes (they go
right-to-left), the C is:

```c
extern unsigned short g_bddc;
extern int g_5370, g_bdd0;
extern void FUN_0003a7c4(int a, int b, int c);

void FUN_00039f49(void) {
    FUN_0003a7c4(g_bdd0, g_5370, g_bddc - 6);
}
```

Compiled with our usual `-4s`, it did not match. The diff was interesting though.
The original pushes the globals *straight from memory* with a single instruction
(`ff35`), and reads the 16-bit global with `movzx`. Ours loaded each global into a
register first and pushed the register, and it zero-extended the 16-bit value the
long way. Different bytes, same behaviour.

Both of those are tells. Pushing memory directly, and preferring `movzx`, is how
Watcom generates code for the **386** (`-3`), whereas the **486** setting (`-4`) we'd
been using loads into registers first, because that pairs better on the newer chip.
The function wasn't wrong C, it was compiled for a different processor level than the
rest of the game code we'd matched so far.

Sure enough, recompiling with `-3s` matched byte for byte.

The lesson, and it refines the discipline from earlier: not every flag is fixed
across the whole game. The *optimisation* setting is pinned, we proved that. But the
**CPU level** is a per-unit choice, like the [calling
convention](calling-conventions.md) is a per-function one. This region of the code,
just below the runtime library, was built for the 386. So when a function refuses to
match on `-4` and the diff shows direct memory pushes and `movzx`, that's not a
reconstruction bug to chase in the C, it's the code telling us its CPU level. Read
it, switch to `-3`, move on. **52 of 500.**

## FUN_00039f69 — the same region has lighter optimisation too

Right next door, this one looked like a near-copy of the last: read the same 16-bit
global, subtract six, push it with two other globals, call the same function. The
only extra was a line at the end setting another global to 1. So I wrote the obvious
C and compiled it with `-3s`, expecting a clean match.

It came out one instruction-pair off. The original does the call, then cleans up the
pushed arguments (`add esp, 0xc`), then does the trailing store. Ours did the call,
the store, *then* the cleanup. Same instructions, swapped order.

That swap is a scheduling choice the optimiser makes: with `-oneatx` on, Watcom
noticed the store didn't depend on the stack pointer and moved it earlier. Turning
the optimisation bundle off (plain `-3s`) put the two back in the original's order,
and it matched.

So this region wasn't only built for a different [CPU](cpu-basics.md) level, it was
also built with *lighter optimisation* than the main game. The previous function
couldn't show that, because it had nothing the optimiser would reorder.

This refines the earlier lesson about [flags](compiler-flags.md). When we said the
optimisation setting is "pinned", that was pinned for the main game's translation
units, the bulk of the code. But separate units, like this `0x39xxx` block just
below the library, can be built with their own settings. Optimisation level, like
CPU level and calling convention, is a per-unit property. The regression test still
protects us: it just means a function in a different unit gets its own recipe, and
the byte diff is what tells us which unit we're in. **53 of 500.**

## FUN_0002d998 — the one that told us the compiler was 9.5

This is the big one, so it's worth telling properly.

`0x2d998` is a small piece of game logic: it recomputes a status code on an object,
using a chain of checks, one of which tests a single flag bit, `if (thing & 0x10)`.
I wrote the obvious C and it came out wrong, eight bytes too long. The difference was
in that bit test. The original tests the flag straight in memory with one
instruction, `test byte[mem], 0x10`. Ours loaded the byte into a register, masked it,
and widened it, three instructions doing the same thing.

I'd seen this shape before, so this time I stopped and probed it properly. I wrote a
handful of tiny functions doing the bit test different ways and compiled them. Every
single form produced the load-and-mask version. Our compiler, Watcom 10.0a at our
usual settings, simply never emits the compact `test byte[mem]` form. So it wasn't my
C. It was the compiler.

Then the key move: I compiled the same probe with **Watcom 9.5** instead, an older
version we had set aside earlier. 9.5 emitted the `test` form, the one the original
uses. So I compiled the real function, `0x2d998`, with 9.5, and it matched **byte for
byte, exactly**.

That's a big claim, so I checked it couldn't be a fluke. I recompiled all 48
functions we'd already matched with 9.5. **Every one still matched.** So 9.5 doesn't
just fix the bit test, it keeps everything 10.0a ever got right.

The honest conclusion: **the game was built with Watcom 9.5, not 10.0a.** The reason
we didn't spot it for so long is that 9.5 and 10.0a produce identical code for most
functions. The simple ones we'd been matching have nothing that tells the two
compilers apart, so they compiled the same either way, and 10.0a looked correct.
`0x2d998` is the first function whose shape actually distinguishes them, and it
pointed at 9.5.

There's a small humbling footnote. We *had* tried 9.5 once before and concluded it
was no better than 10.0a. That test was run against the wrong functions, framed
runtime-library code that fails on both compilers for reasons that have nothing to do
with the version, like a different register-save order. Testing on clean game code
with a bit test would have shown it immediately. A good reminder that a negative
result is only as good as the case you tested it on.

So from here the harness switches to 9.5. It unlocks flag tests and bit-field checks,
which are everywhere in game logic, while keeping every match we already have.
**56 of 500**, and the road ahead just got wider.
