# Reverse-engineering journal

A running log of the actual work: what we took on, why, what went wrong, and how we
got there. It's the narrative version of the terse session log in `AGENTS.md`,
written so the thinking is easy to follow later. It reads in order, oldest first,
and each entry tries to show the *process*, not just the result.

The arc runs from the first tiny functions that matched straight away, through the
discovery that the compiler was Watcom 9.5, into the register-allocation walls that
clean C can't cross, out the other side into reading the code for what the game
actually does, and finally into standing the whole reconstruction up and running it.
Each entry below is one step of that.

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

## FUN_00039f49, a "clean" function that taught us about CPU levels

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

## FUN_00039f69, the same region has lighter optimisation too

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

## FUN_0002d998, the one that told us the compiler was 9.5

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

## FUN_00033fb8, a map passability check, one register short

This one didn't fully match, but it's worth recording because it shows both how far
reading the diff gets you and exactly where the hard wall is, and because we worked
out what the function actually does.

It's a piece of the map system. Given a world position (x, y, z), it finds the map
tile under that spot and returns whether the tile is walkable. It gets there with a
column index from x, a row from y, a per-column base offset from a table, then reads
the tile byte, looks its type up in a second table, and checks whether the type is
in the walkable set (6 to 9, or 11).

My first attempt was structurally wrong, but the diff walked it in. Swapping the
order of two calculations fixed a whole cascade (the compiler had allocated registers
differently because I computed the column before the row). Inlining a lookup instead
of storing it in a local fixed another. Those two changes took it from a completely
different function down to a single byte.

That last byte is register allocation, the wall we keep meeting. The original holds a
pointer in one register and combines it with a `lea`. Ours puts it in a different
register and uses an `add`, which happens to be one byte shorter. Same result,
different register, and there's no clean lever in the C to change which register
Watcom picks. So this sits at ninety-nine percent, parked with the other
register-allocation cases until we find a way to crack those.

Still a win of a kind: we understood a real game system function, even though its
bytes aren't matched yet. That understanding goes in
[how the game works](game-systems.md).

## FUN_000377b8, a chain counter that cracked the loop wall

After the register-allocation wall we needed a win, so I went looking for a clean
function with no calls, and found this one. It walks a linked chain and counts how
long it is. It reads a 16-bit id from a field on the object passed in, and while
that id isn't zero it looks the node up in a table, follows the link stored at the
node's `+0x1c`, and counts one more hop. When the id runs out, it returns the count.

The first attempt was the obvious C, a `while` loop with `count++` inside, compiled
with our usual `-oneatx` optimisation. It came out badly wrong, and the diff showed
three separate problems stacked on top of each other. The registers were swapped,
the original kept the id in EAX and the count in EDX and ours did the opposite. The
loop was shaped differently, the original tested the condition at the top and jumped
back up, ours tested at the bottom. And the address arithmetic was folded together
where the original kept it in two steps. Three walls at once.

I unpicked them one at a time, and each fix taught something.

The loop shape came first. Our `-oneatx` build had *rotated* the loop, moving the
test to the bottom, which is a normal optimisation but not what the original did. The
original's loop tests at the top with an unconditional jump back, the un-rotated
form, which is what you get with lighter optimisation. Dropping from the full
`-oneatx` bundle to a plainer setting brought the top-test shape back, and as a bonus
the register swap fixed itself, because with the lighter optimiser the id naturally
landed in EAX like the original.

That left two small things. The original does the address in two instructions, load
the table base then read the link, so I gave it a named pointer local instead of one
folded expression, and the two steps came back. And there was one stubborn ordering
difference, the original increments the counter *before* reading the next link, ours
did it after. That's a scheduling choice, and it turned out to be sensitive to the
exact optimisation flag. Walking through the reorder options, `-or` put the increment
in the right place.

There was one more subtlety worth recording. Even at the lighter setting, the
original has what looks like a redundant test, it checks the id once before the loop
and again at the top of the loop, two identical tests in a row. Our plain `while`
only produced one. That shape comes from an `if` wrapped around the `while`, a guard
the compiler didn't fold away, so writing the C that way, `if (id) while (id) {...}`,
reproduced it exactly.

With the top-test loop, the explicit pointer local, the `if` guard, and the `-or`
flag, it matched byte for byte. The lesson is the useful one, the loop-rotation wall
isn't a dead end like the pure register tie-break in `0x33fb8`. It responds to two
levers we can actually pull, the optimisation level and the way the loop is written
in C. This unit, like the `0x39xxx` block, wants lighter optimisation than the main
game, and the byte diff is what tells us so. **57 of 500.**

## FUN_000377e8 and FUN_00014998, inlining as a register lever

These two came next door to the chain counter, and both taught the same small trick.

`0x377e8` is a map lookup. It reads a tile id from a field, turns it into a pointer
into a table, bounds-checks it, and if it's valid reads a byte and translates it
through a second table. My first C had the right logic but three separate byte
differences: the pointer sat in the wrong register, the 16-bit read was widened the
long way, and the early return was in the wrong place. The fixes were instructive.
Getting the return type right (a 16-bit `unsigned short`, which the original's
`xor ah,ah` tail gives away) fixed the return. Inlining the intermediate values
instead of naming them in locals fixed both the register choice and the widening
idiom in one go, the compiler stopped shuffling the value between registers when the
expression was one piece. And the early return only fell into the right place once I
wrote it as `if (valid) return lookup; return 0;` rather than `if (invalid) return 0;`
first. Same logic, but the second form tells the compiler which path is the cold one,
and it parks the `return 0` at the bottom exactly like the original.

`0x14998` is a chain walk again, this time through fifteen-byte records, following a
link field until it hits zero. It sits in the main game so it wants the normal
optimisation, and the loop shape matched first time. The only snag was the same
widening idiom, my named `id` local made the compiler load the value into one
register and copy it to another. Inlining the read straight into the multiply let it
load directly into the right register, and it matched.

So the running lesson from this pair, an intermediate local isn't free. Naming a
value can force the compiler to hold it somewhere and move it, where inlining lets it
flow straight into where it's used. When the diff is a stray register copy or a
widen done the long way, try collapsing the locals before reaching for anything
cleverer.

There were two near neighbours we couldn't take. `0x37818` shares its `return 0` with
the function physically above it, its out-of-range branch jumps into that neighbour's
tail, so it can't be matched on its own, only as part of a larger unit. And `0x37738`
came down to a single idiom, the original holds a field in a preserved register and
widens it, ours keeps it in the accumulator, and no rearrangement of the C or the
flags moved it. That's the register-allocation wall again, so it's parked. **59 of
500.**

## FUN_00014cc8 and FUN_00016638, types and layout tell you more than you'd think

Two small loops that both hinged on details the disassembly quietly announces.

`0x14cc8` scans a few fixed-size records for a flag bit and returns yes or no. The
first version had the right logic but returned through the full accumulator where the
original only touches the low byte, and it counted with a 32-bit register where the
original used a byte. Both are the same tell: the original works in bytes, so the
counter is a `char` and the return type is a `char`. Switching to those brought the
byte-sized instructions back. The last piece was where the compiler put the `return 1`.
Written as a `while`, our exit ended up at the bottom, reached by a jump, where the
original keeps it inline right after the loop guard. Rewriting the loop as
`for (;;) { if (done) return 1; ...body... }` put the early return exactly where the
original has it, and it matched.

`0x16638` finds the count-th entry in a table matching a value, with an index that
wraps at fifty. It taught two sharper lessons. First, **Watcom's `char` is unsigned by
default.** Our index compiled to unsigned instructions, `jb` and a zero-extend, where
the original uses signed ones, `jl` and `movsx`. Declaring the index `signed char`
fixed that in one stroke, a reminder to read the *signedness* off the diff, not just
the width. Second, the count-is-zero guard: the original returns the same value as the
normal exit and shares one return at the bottom, but because our early `return idx`
happened when the compiler knew the index was still its start value, it folded it to a
constant and wrote a second return. Restructuring as
`if (count) { do ... while (count); } return idx;` gave a single shared exit like the
original.

There was one genuinely instructive flag moment. The wrap-to-zero inside the loop was
being hoisted, the full `-oneatx` optimiser lifted the zero constant into a spare
register once and reused it, where the original just zeroes the byte inline each time.
That hoist is the `x` in the bundle. Dropping to `-ot`, which still gives the fast
`lea`-based address arithmetic the original uses but doesn't do the loop-invariant
hoist, matched exactly. So this unit, like the others we've found, was built with
lighter optimisation than the main game, and the specific instruction that gave it
away was a constant that should have been inline. **61 of 500.**

## FUN_00013a98, when the explicit local is the answer

Earlier the lesson was that naming a value can hurt, that inlining lets it flow into
the right register. This one is the mirror image, and it's worth pairing them so the
principle is clear rather than the recipe.

`0x13a98` checks a flag on one of its arguments, and either forwards to another
function and returns zero, or returns one. My first attempt wrote the two outcomes as
two `return` statements. It was close but wrong in three linked ways. The original
holds the result in a single register the whole way through, and that one register
does triple duty, it starts as one for the "return 1" case, it's reused as the mask
for the flag test (`test [reg+0xb], bl` rather than a fresh immediate), and it's
zeroed before the call for the "return 0" case, with a single `mov al, bl` exit at
the bottom. Two separate returns can't express that, because the value never lives in
one place.

Writing it with an explicit `result` variable, set to one, flipped to zero inside the
branch, and returned once at the end, gave the compiler exactly that single-register
lifetime. The test-mask reuse appeared, the shared exit appeared, and as a bonus the
third call argument moved to the register the original uses, because the result was
now occupying the one ours had wrongly borrowed. It matched.

So the pair of lessons sits together. Inline a value when it should flow straight
through an expression into where it's used. Name a value when it needs to persist in
one place across branches or a call, especially when it's also a return value. The
diff tells you which, a stray copy or a long-way widen says inline, a value that
should have stayed put across a call says give it a name. **62 of 500.**

We also looked at `0x20d18` and left it. It's a satisfying one to understand, it's
the map initialiser, it walks a twelve-thousand-entry table turning stored offsets
into real pointers and then publishes the table's base in the same `g_5358` the
[passability check](game-systems.md) reads, so this is the function that *builds*
what that one *reads*. But it compiles to an alignment-padded loop, the compiler
inserts do-nothing instructions to line the loop up on a boundary, and it keeps the
base in a preserved register with an explicit pointer walk rather than the folded
addressing ours used. Reproducing padding that depends on the function's absolute
placement is fragile in isolation, so it's parked as understood-but-unmatched, with
its purpose recorded because that's the half that lasts.

## The do-while correction, and matching on canonical flags

A scepticism check paid off here. We had a handful of functions recorded with lighter
optimisation than the main game, and it looked like the binary was built from several
translation units with different settings. Pushed to prove it, most of that evaporated.

The key case was `0x14998`, a chain walk I'd written as a `while` loop. It appeared to
*need* the main optimisation level, because only that level matched. But the real shape
was a `do-while`, and once written that way it matched under both settings. A
`while`-that-should-be-a-`do-while` forces the compiler to rotate the loop, and only
the heavier optimisation rotates, so a wrong loop shape masqueraded as a build
difference. That's a humbling and useful lesson: a loop's `while`/`do-while`/`for`
shape is a variable to get right in the C, not a signal about compiler flags.

With that understood, the discipline became: assume one canonical build, and treat
every mismatch as our C until proven otherwise. Two functions in the "lighter" region
then fell to canonical flags with better C. `0x376f8` (a sum over the same chain
`0x377b8` counts) matched once its return type was an `int` over a 16-bit accumulator,
the giveaway being a zero-extend of the low sixteen bits on the way out. `0x28cc8`, a
routine that builds two local buffers and calls out, matched once the two buffers were
declared in the right order, because the compiler lays locals out in reverse of
declaration, so swapping their order swapped their stack offsets to match. The
optimisation picture collapsed from five flag sets to one canonical setting plus two
lone holdouts still stuck on register placement.

## The register walls, two cracks, and finding the floor

By this point the easy half was done and what remained were the *walls*: functions
that decode to correct C but whose bytes we cannot reproduce because the difference is
something the compiler decides internally. Three shapes recur. **Register-role**: the
same instructions, but a value the game keeps in `ESI` we keep in `EDI`, or a register
gets pushed on one side and not the other. No C spelling flips which register the
allocator picks. **Scheduling**: two independent instructions in the other order, like
a `[esp+4]` load that always lands before `[esp+8]` no matter how we write the source.
**Encoding tie-breaks**: `cmp a,b` versus `cmp b,a`, an `imm8` versus an `imm32`, a
byte cleared with `xor dh,dh` versus `xor dh,ah` because the compiler noticed `ah`
already held the same value. We proved these are not our C by exhausting the levers:
the source permuter, hundreds of flag combinations, and seven Watcom builds all leave
them unchanged. A wall that survives every declaration permutation is the allocator's
choice, not unfound C, and the honest move is to record what the function *does* and
stop grinding.

But two things that *looked* like walls turned out to have a lever, and finding them
was the satisfying part.

The first was a twenty-two-byte function that just calls another function behind an
`if`. The target wrapped the whole body in a `push ebx … pop ebx` around code that
never touches `ebx`. A dead save. The reason is a contract: this function promises
*its* caller to preserve `ebx`, and it calls something that clobbers `ebx`, so it must
save it across the call, across the whole body, since that is all there is. Every
attempt to make our C use `ebx` added real instructions. The only way to get a *dead*
save is to declare the callee's clobber, `#pragma aux <callee> modify [ebx]`, which is
exactly what the original translation unit's header would have said. It matched. It is
the only C that produces those bytes, and it is honest. That pragma is the callee's
real ABI.

The second was better. A pool accessor whose "return 0" had no return of its own. It
jumped *backwards*, into the return stub of the function physically before it. Two
sibling accessors, near-identical, and the compiler had noticed their return-zero tails
were the same bytes and written it once, letting the second borrow the first's. That
cross-function merge only happens when both are in one object, so matching it meant
abandoning one-function-per-file and compiling the two siblings together in a single
translation unit, the stub-owner first. It still needed one nudge: the borrower had to
return `unsigned short`, not `unsigned char`, so its return-zero stub was byte-identical
to the sibling's and could actually be shared. Then the whole region matched, backward
jump and all. It is the only match so far that required thinking about two functions at
once, and it opened a small piece of new machinery, source files that hold a whole
module, verified as one contiguous region.

We went looking for more of that shape and found it was the only one. A scan for jumps
that leave a function's own body turned up nothing else. So the whole-module lever
bought exactly one function here, though on a game built from larger modules it could
buy many, which is why it is written down.

That left the compiler itself as the last suspect. The bulk matches Watcom 9.5b, which
is strong proof of the family, yet a handful of walls carry a *newer* tell, the
`add eax, imm32` accumulator form that only 10.0 emits, where 9.5 always shrinks to
`imm8`. So the game sits in a seam: 9.5's register allocation with an occasional 10.0
peephole. We chased the obvious candidate, 9.5c, built it from a patch, and it was
byte-identical to 9.5b on every wall. 10.0a has the newer encodings but breaks
functions 9.5 matches. We even pulled Watcom C/386 8.5 to rule out the older direction,
and confirmed what the tells already said: the walls need something *newer*, not older,
so 8.5 could only regress the bulk. The exact build appears to be a transitional release
that was never archived separately. With that, the version hunt is closed: this is the
floor. Every remaining function is decoded, understood, and byte-exact in the build
through a transcribed-bytes fallback. The clean C is as close as the compiler we can
run allows, and the purpose of each function, the half that lasts, is written down.


---

## The systems behind the walls, is this really the whole game?

With the matching floor reached and the compiler question closed, the work changed
character. The question stopped being *can we reproduce the bytes* and became *is this
genuinely the whole game?* The doubt was concrete and fair: someone remembers the cars
you steal and drive, the Persuadertron that turns a crowd into your private army, the
flamethrowers and rail guns. If OBJECT1 is 261 KB of code, where in it are those things?
Answering that meant reading the code for *meaning*, not just for bytes, and it turned
up both the systems we were looking for and a run of our own earlier guesses that were
wrong.

**The Persuadertron.** Every live entity runs a behaviour chosen by a state byte through
a jump table (`entity_behaviour_dispatch`, 0x2ea88), called once per entity by the
per-frame loop (0x31858). One of those behaviours, `persuade_capture` (0x2fe68), is the
convert step: when the agent reaches its target ped, it sets the ped's *leader link* to
the agent, chains the ped into the agent's follower group, raises a "controlled" flag,
and clamps a counter through a per-type table. The persuaded ped then runs the
follow-leader behaviour and trails you. That was the moment the whole entity model paid
off. Allegiance is a single field flip, and the iconic weapon is just a per-state
behaviour reaching into the same pool-and-handle machinery as everything else.

**The first humility.** That per-type table, `g_a73a`, carried a tentative name from an
earlier pass: "persuade limit." The persuade code *does* use it as a cap, so the name
looked confirmed. But decoding the equipment screen showed the same table stocking ammo
quantities, and the new-game loadout drawing starting weapon counts straight out of it.
It was never a persuade table, it is the per-item-type *maximum quantity*, and persuade
merely clamps a shared amount field through it. Renamed `g_item_max_qty`. The address was
always the anchor. The name was a guess, and the guess was wrong. It would not be the
last.

**Where the weapon numbers live.** The correction opened a bigger question: where are the
stats (damage, range, ammo)? Read the bytes at the tables that hold them and they are
zero. Not missing, just *zero in the shipped image, filled at runtime*. Following who fills
them led to the resource loader: `validate_records_or_abort` (0x18338) walks a list of
block descriptors, `realloc_block_descriptor` (0x184b8) opens each named file and reads
it, and when the read comes back short, because the file was compressed, a decompressor
expands it in place. That decompressor checks a four-byte magic, `RNC\x01`, and runs a
Huffman-plus-back-reference bitstream: Rob Northen Compression, method 1, the standard
packer of its era. So the balance data was never in the code. It lives in RNC-packed
`data/*.dat` files (their names sit in a table in OBJECT2, alongside the multilingual
equipment descriptions), and OBJECT1 holds only the logic that loads and reads them. That
is the honest answer to the original doubt: the *code* is complete. The *numbers* were
always external, which is exactly why a table can read as all-zeros and nothing be missing.

**The vehicles.** The engine keeps five object pools back to back, one per class, each
record tagged by a kind byte, and the record counts lock onto the level arrays exactly:
256 people, 64 cars, 400 statics, 512 weapons, 256 effects. Cars are their own pool
(B, at 0xdd10, kind 5): each body carries per-model hit points, is redrawn every frame by
a dedicated tick, and can anchor itself to a pool-A driver so the body follows the person.
Boarding is a doubly-linked occupant list hung off the car. Driving is a real speed model
(accelerate, brake into corners, cap at a max) that steers by following directional
road tiles (a tile's value encodes which way traffic flows through it). Trains and boats
are the same machinery with the rider hidden. It is all there, and it is all built from
the same entity primitives as the peds.

**The reckoning.** Finding the vehicles cost four earlier names. What a previous pass had
labelled `weapon_fire` was the vehicle *drive step*. `formation_follow`, `join_new_leader`
and `detach_entity_type` were the *ride*, *board* and *exit* handlers. The byte matches
were never in doubt, those functions still reproduce the original exactly, but the
labels were speculation from a naming sweep that ran ahead of the analysis, and
speculation, read back months later, reads as fact. This is the lesson worth keeping: a
name is a claim, and a claim you have not traced is a liability, because the next person
(or the next you) trusts it. The byte-level matching is the durable, verifiable half of
this project. The semantic naming is the fragile half, and it's worth having only when
each label has actually been followed to its callers. We corrected the four, wrote down
what is confirmed versus inferred, and left the entity model more honest than we found it.

A smaller lesson came from the build itself. For several commits the whole-program size
was quoted as unchanged. That was measured, it turned out, against an *incremental* object
cache that had quietly accumulated correctly-built objects over the project's history. A
build from a clean slate is smaller and is the real, reproducible number. The identifier
renames are byte-neutral either way, but the moral holds: verify against the clean build,
not the one your tools happen to have lying around.

---

## Running the reconstruction, and a root cause that wasn't

For most of this project the question has been "do the bytes match". This stretch asked a
different one: does the whole thing actually run?

It does, and further than expected. The reconstructed executable boots under DOS/4GW and
draws the Bullfrog intro, the Syndicate main menu, and the world map, pixel for pixel with
the original. It takes input too. The menu responds to its function keys and the map to
mouse clicks, so a scripted session walks title to menu to map on its own.

**But two things are missing: the text and the sound.** The map draws its coloured
territories and the population figure, yet not a single word of it, no region name, no
BRIEF or MENU button, no date. The original draws all of that. There's no audio either. The
odd part is that the menu text a screen earlier renders perfectly, so the font engine works.
Some text lives, some doesn't.

The right tool for "same code, different behaviour" is a trace diff. Our build and the
original share the exact same machine code for the game. Only the reconstructed data differs.
So we ran both under a patched DOSBox that logs every instruction executed, launched them as
the same filename so nothing external could differ, and looked for the first place the two
streams part. That point is where a wrong byte of data first changed a decision. The two ran
in lockstep for about a million and a half instructions, then split inside the game's
decompressor, the routine that unpacks Bullfrog's RNC-compressed resources. So a resource was
coming out wrong, and since the graphics unpack fine, only some of them.

**Then the part worth writing down.** We dumped the decompressor's state at the split. The
compressed input was identical in both, byte for byte. The code was identical. The only thing
that differed was a single word of the decompressor's bit buffer that the routine reads before
it ever sets it. We chased where that word came from, and it led somewhere strange: at startup
the game shells out and runs the intro as a child process, and that child scribbles over the
parent's low memory, right where the decompressor keeps its scratch. Our leftover there
differed from the original's, and the decompressor read it. It was a tidy story. An
uninitialised read, dormant in the original for thirty years, woken up by our slightly
different memory.

**The trouble is it was wrong.** To prove it we forced that word to the value the original
happens to have, zero, and ran again. The map stayed blank. The decompressor still built an
empty table. And when we looked once more, the compressed input matched, the initialised state
matched, the one suspect word now matched, and the output still came out different. So that
word was a symptom travelling alongside the real fault, not the fault. The decode still
diverges from something we haven't found, and the root cause is open.

The lesson is the naming lesson wearing a new coat. A plausible cause, traced through half a
dozen memory dumps, reads like a conclusion long before it is one. It only becomes one when a
fix confirms it. We would have written a confident, wrong "root cause" into the notes if we
hadn't run the one experiment that could disprove it. The trace-diff machinery is the durable
half of this, it will find the real input next time. The story we told ourselves about that
one word was the fragile half, and the honest entry is the one that says we found the
neighbourhood, not the culprit.

A day later even the neighbourhood turned out innocent. We finally dumped the decompressor's
*output*, the bytes it actually produces, in both builds, and they were identical, byte for
byte. The decompressor was never wrong. What the instruction trace had flagged was our copy
reading a little further ahead into the very same compressed stream, so it topped up its bit
buffer one instruction sooner. Same data in, same data out, different rhythm. The first place
two runs of the same code part company is not automatically the bug, because a difference that
only changes how far ahead you read perturbs the timing without touching the result. The real
gap is somewhere the runs part and stay parted, and the honest measure was never the
instruction stream, it was the output. Lesson relearned, at some expense: diff what the code
*produces*, not just what it *does*.

## The blank map explained, a hole in the data we were building

The root cause was never in the game code. It was in how we rebuild the data.

Here is the shape of it. The reconstruction assembles DGROUP, the game's initialised data, from two dumps we call OBJECT2 and OBJECT4. Both dumps are cut 0x28b8 bytes into the real object, the same stub-sized cut that afflicts the code dump. We knew that, and we placed each dump 0x28b8 bytes in to line the rest up. What we did not carry was the missing prefix. So the first 0x28b8 bytes of DGROUP, everything below that offset, sat as zeros in our build. Real data lives down there: the table of "data/xxx.dat" filenames, the run-time library's error strings, and a two-byte magic the sound loader checks. All of it read back as zero.

That one hole explains both symptoms at once. The sound loader reads a container's version, compares it against the expected magic held at DGROUP offset 0xb0, and bails if they differ. In the original that magic is "LX". In our build offset 0xb0 was zero, so the compare failed and sound init gave up. The same blank region starved whatever the text drawing leans on, and the map came up wordless.

We nearly missed it because of a puzzle that looked like a contradiction. The compare passes a pointer the disassembly prints as a bare 0xb0, yet a run-time stack dump from the previous session had read it as a heap address near 0x1c40e0. For a while those two readings seemed to fight each other. They do not. That push is relocated. The static image stores the DGROUP offset, 0xb0, and the loader fixes it up to the real address, base plus 0xb0, when the program loads. Both numbers were right all along, one before relocation and one after. The fixup table confirmed it, a thirty-two bit source at exactly that spot targeting object two, offset 0xb0.

The fix is to stop using the cut dumps for data and read the objects straight from the executable's data pages, the same route we already use to rebuild the code. That hands back the full object from offset zero, prefix and all. We checked it does no harm above the cut, the recovered bytes match the old dump at every position we were already getting right, and it fills the part we were getting wrong. Offset 0xb0 now reads "LX", followed by the start of "not enough memory to allocate", which is just the next string sitting behind it.

Rebuilt, the intro speaks. The subtitles render, "DATELINE :1/85 NC", "TIME : 18:20 HRS", where before there was blank space. That is the honest extent of what we have watched with our own eyes. Two things are argued but not yet seen: the sound, because the capture rig runs a dummy audio device, and the world map's own labels, because we could not drive a keypress past the intro under the headless display to reach the menu. The mechanism is proven by the data. The last two confirmations are a matter of getting the harness to cooperate.

There is a lesson here that rhymes with the last one. The previous entries kept mistaking a symptom near the fault for the fault. This time the trap was the opposite, a real contradiction that dissolved once we understood relocation. The blank data was measurable all along. We found it not by staring harder at the instruction trace but by asking what a specific pointer actually pointed at, and then checking whether the byte it named was there. It was not. Everything else followed.

## The map speaks, and a harness taught to click

The last entry ended on a promise we had not yet kept. The data fix was proven, the intro spoke, but we had not watched the reconstruction reach the world map with its own labels, and we had not been honest until we had.

Getting there was not a decompilation problem at all. It was the harness. The game runs headless on a virtual display, and driving it means sending keystrokes and clicks from outside. The keystrokes went nowhere. The reason is small and worth remembering: the emulator is an SDL programme, and SDL only accepts real input events delivered to the window that holds the keyboard focus. On a bare virtual display there is no window manager to hand out focus, so the events fell on the floor. Running a tiny window manager, one that lets focus follow the pointer, and parking the pointer over the game, fixed it in one line. A test proved it: typing a directory command at the DOS prompt now echoed where before it had done nothing.

The mouse was its own puzzle. The game draws its own cursor and moves it under its own mapping, not one-to-one with the host pointer, so a click at the place we meant landed somewhere else. We measured two points, fitted the scale and offset, and could then put the cursor on a menu item on demand. The first attempt clicked "quit to dos" instead of "begin mission" because our vertical scale was off. The second landed on the load-and-save screen, one row low. The third hit "begin mission", and the world map came up.

It renders. "Western Europe", the population figure, the funds, the region ownership, the brief and menu buttons, all in the sharp green type of the original. This is the exact screen that used to come up with blank panels, the "no text on the dashboard" that started the whole thread. The hole in the data is filled, and the game shows it.

The lesson from the harness half is one we keep relearning in different clothes. When a thing that should work does not, the fault is often below the thing you are looking at. We spent effort suspecting the game before noticing the input never reached it. The cheapest test, typing at a prompt, would have told us that on the first day.

## The sound that would not play, and a heap pushed out of place

With the map reached, one thing still failed: turn the sound on and the game crashes on the way from the intro to the menu. We traced it a long way, and the trace is a good example of ruling things out by value rather than by suspicion.

The crash is a jump through a garbage function pointer inside a sound-driver timer callback. The first instinct is that we install the wrong pointer, so we dumped the driver's dispatch tables from both our build and the original. They are identical, relocated to each build's load address but otherwise the same bytes, and the pointer we install is the correct handler at the same offset into the driver image. The callback is right. The crash is in running it.

The one thing that differs is where the driver is loaded. In our build it sits about a megabyte higher in the heap than in the original. We chased that, and the malloc trace is clean and damning at once: the sizes and the order of every allocation match the original exactly, but the addresses drift. The heap has two arenas, small blocks low and large blocks high, and the large arena begins immediately after the game's initialised data. Our initialised data is about eight hundred kilobytes too large, so the large arena, and the sound driver in it, starts that much too high, at an address where the driver misbehaves.

The oversize is a mistake we made for a good reason. When we size the data segment we made it cover every global the code names, including one very high address that turns out to be the C run-time's heap control block, sitting well past the real data in the heap rather than inside it. Covering it in the data segment is wrong, and it is exactly what pushes the heap out of place. So we tried the obvious fix, size the segment to its true length and let the run-time own the heap. The game aborts at once. That high address is now unbacked memory and the heap initialisation faults on it. So the two mistakes hold each other up: the large segment backs that address but shifts the heap, and the true-size segment aligns the heap but leaves the address hanging.

We stopped there, with the cause understood and the easy fix disproven. The real repair is to size the data correctly and separately back the heap region so the run-time's near heap is valid and grows as it did in the original. That is a careful change to how we lay out memory, and it deserves doing deliberately rather than at the tail of a long day. The sound is not fixed. But it is no longer a mystery, and the shape of the fix is known.

## The floor, and knowing when to stop

The last thread was the opposite of a fix. We were asked to reach a hundred per cent match, and the honest answer took work to earn.

The tempting tool was a permuter that rewrites register assignments in the compiled object to reach the target bytes. We scoped it before building it, which saved building it. A register-normalised diff over all hundred-and-eleven functions still short of a match found not one that a pure renaming could fix. They diverge in the shape of the instructions, not just the names of the registers, and that shape is driven by the compiler's own allocation choices, which no source spelling steers. We pushed on the closest handful anyway, including one that differs by a single `lea`, and every spelling folded back to the same bytes.

So the number will not reach a hundred per cent from compiling C, and now we can say that as a measurement rather than a mood. The finding is written up properly in [register allocation](register-allocation.md). What is worth recording here is the discipline of it. The useful outcome of scoping a tool is sometimes learning not to build it, and the useful outcome of grinding a wall is sometimes a clean proof that it is a wall. Both are progress, even though neither moves the score. The decompilation is complete, every function decoded to readable C, and the reconstructed binary is byte-exact because it is built from the original bytes. The one claim we cannot make in full, that our C recompiles to every exact byte, is capped by the compiler, not by us.
