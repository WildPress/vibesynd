# Compiler flags

A **compiler flag** is an option you pass to the compiler to change how it turns
your C into machine code. The same C, compiled with different flags, produces
different bytes. Since we need the *exact* original bytes, we have to use the exact
flags the original was built with.

The flags we settled on are `-4s -oneatx -zp8 -s -zq` (with `-4r` instead of `-4s`
for register-calling functions). Here's what each part means and how sure we are of
it.

## What each flag does

- **`-4`**: the CPU level. It tells the compiler which processor to optimise for.
  `-4` means the 486. This matters because newer chips have instructions the
  compiler will happily use if you let it, and the original doesn't have them. We
  know it's `-4` because one function uses a `movsx` instruction that the Pentium
  setting (`-5`) would replace with something else.
- **`s` or `r`**: the calling convention, stack or register. This is the one thing
  that varies per function. See [calling conventions](calling-conventions.md).
- **`-oneatx`**: the optimisation bundle. Optimisation is the compiler working
  harder to produce faster or smaller code. `-oneatx` is a particular set of
  optimisation options bundled together. Different bundles produce different code.
- **`-zp8`**: struct packing. This controls how the compiler lays out the fields of
  a struct in memory, specifically how much padding it inserts between them.
- **`-s`**: turn off stack overflow checks. Release games disabled these, and if
  they were on we'd see extra checking instructions in every function's prologue,
  which we don't.
- **`-zq`**: quiet mode. Just tells the compiler to print less. It has no effect on
  the output bytes.

## How we worked out the flags were right

This is the interesting part, because we didn't just assume. We built a way to
check.

The idea is a **regression baseline**. We already have a set of functions that
match. If a candidate set of flags is correct, it has to still reproduce *all* of
those. So for any flag we want to test, we recompile every existing match with it
and count how many still pass. Anything that breaks an existing match is wrong,
because we know those functions are genuinely correct.

Running that check settled things:

- The optimisation bundle `-oneatx` is **pinned**. Every alternative we tried
  breaks matches we already have. `-ox` breaks a few, `-ot` breaks more, `-os`
  breaks a lot. So `-oneatx` isn't just one setting that happens to work, it's the
  one that survives when the others don't.
- The CPU level is `-4`, pinned by that `movsx` instruction.
- Stack checks are off, because we never see the checking instructions.
- Struct packing (`-zp8`) is the one flag we *cannot* confirm from what we have.
  `-zp8`, `-zp4`, and `-zp1` all keep every existing match, because none of our
  matched functions have a struct whose layout would change between them. So we
  assume `-zp8` but haven't proven it. We can sidestep it anyway by writing struct
  layouts out by hand.

The tools for this are `tools/recipes.py`, which records the flags each match
needs, and `tools/caltest.py`, which tries a candidate set against the whole
baseline.

## Why this discipline matters

Pinning the flags does something quietly important. Once you know the flags are
right, a function that comes out wrong can only mean one thing: your C is not what
the original author wrote. It can't be blamed on a flag any more.

So instead of fiddling with flags when something doesn't match, we hold them fixed
and work on the C. The byte difference becomes a clue about the original source,
not an excuse to go flag-hunting. That's the right way round for a matching decomp.
