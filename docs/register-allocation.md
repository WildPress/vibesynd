# Register allocation

This is the single hardest thing to match in the game's own code, so it's worth
understanding why.

## What registers are, again

A [**register**](cpu-basics.md) is a tiny storage slot inside the CPU, much faster
to use than memory. The x86 chip has only a handful of general-purpose ones: EAX,
EBX, ECX, EDX, ESI, EDI, and EBP. A function juggles its values between these
registers and memory as it runs.

## The allocation problem

When the compiler turns your C into machine code, it has to decide **which
register holds which value** at each moment. A function might have a pointer, a
loop counter, and a running total all live at the same time, and only so many
registers to put them in. Choosing the assignment is called **register
allocation**, and it's one of the cleverer jobs a compiler does.

The important thing for us: there's often more than one *correct* answer. Putting
the counter in EBX and the total in ESI works exactly as well as the other way
round. The program behaves identically. But the *bytes* are different, because the
instructions name different registers.

## Why it's our main wall

The compiler is deterministic. Give it the same C and the same flags and it makes
the same choices every time. So if our compiled function puts a value in a
different register than the original does, that isn't randomness. It means our C is
different enough that the compiler's allocation logic went a different way.

Sometimes we can nudge it back by rewriting the C. Sometimes we can't, because the
allocation hangs on something subtle that's hard to express in source. That's the
wall: the logic is right, the behaviour is right, but the compiler parked a value
in EBX where the original used ESI, and everything downstream shifts.

## The pattern that predicts success

Here's the useful rule we found. It comes down to how much *freedom* the compiler
has.

- **Forced allocation**: when there's really only one sensible place for each
  value, the compiler has no choice, so it makes the same choice the original did.
  These functions match on the first try. Forwarders that hand their arguments
  straight on, initialisers that fill in a struct, linear sequences of calls,
  simple setters. There just isn't room to diverge.
- **Free allocation**: when two or more values could each go in several registers,
  the compiler picks, and it might pick differently than the original's compiler
  did from our slightly different C. These are the ones that wall us.

So the practical strategy is to target the forced-allocation shapes, where success
is reliable, and treat the free-allocation ones as the slow tail, where we have to
work the C carefully or accept that some won't yield.

## Why we don't just change flags

It's tempting, when a register is in the wrong place, to try a different flag and
see if it shifts. We learned not to. The flags are pinned (see
[compiler flags](compiler-flags.md)), so bending them to force a register into
place would just be papering over a wrong reconstruction. The honest signal is that
our C differs from the original's, and the fix, when there is one, is in the C.
