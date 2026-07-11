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
