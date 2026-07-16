# The CPU, registers, and instructions

Everything else in these notes sits on top of a few basics about how the processor
works. This is the page to look them up, and the other pages link back here for the
terms.

## What the CPU does

A CPU runs a long list of tiny **instructions**, one after another. Each does
something small: move a number, add two numbers, compare them, jump somewhere else.
A program, including this whole game, is nothing more than a very long list of them.

The chip this game runs on is an **x86** processor, in its 32-bit form (also called
i386 or IA-32). x86 is the family of chips in most PCs of that era and since.

## Registers

A **register** is a tiny storage slot inside the CPU itself. There are only a
handful, but they're the fastest storage the CPU has, far quicker than reaching out
to main memory. The CPU does its work by shuffling values between registers and
memory.

The 32-bit x86 has eight general-purpose registers. Their names start with E, for
"extended", meaning the 32-bit versions:

| Register | Name | What it's usually for |
|---|---|---|
| **EAX** | accumulator | arithmetic, and a function's return value |
| **EBX** | base | general use, often a value kept across a call |
| **ECX** | counter | loop counts and shifts |
| **EDX** | data | arithmetic, and a second return slot |
| **ESI** | source index | the source pointer in copy loops |
| **EDI** | destination index | the destination pointer in copy loops |
| **EBP** | base pointer | the "stake" for a [stack frame](stack-frames.md) |
| **ESP** | stack pointer | always points at the top of the [stack](#the-stack) |

The roles in the last column are conventions, not hard rules. The compiler uses the
registers however suits it, and working out which value it parked where is a big
part of the job (see [register allocation](register-allocation.md)).

You'll also meet half-width names. The low 16 bits of EAX is **AX**, and AX splits
into **AH** (its high byte) and **AL** (its low byte). So `mov al, 3` sets only the
lowest byte. The same pattern applies to the others: BX, BH, BL, and so on.

## The stack

The **stack** is a region of memory for temporary things: a function's arguments,
its local variables, and registers it wants to save. It grows and shrinks as
functions call each other, and **ESP** always points at its current top. The stack
is central to [stack frames](stack-frames.md) and
[calling conventions](calling-conventions.md).

## Instructions you'll see a lot

A typical function uses only a small vocabulary of instructions:

- `mov dst, src`. Copy a value from `src` to `dst`.
- `push x` and `pop x`. Put `x` on the stack, or take the top of the stack into `x`.
- `call f`. Jump into function `f`, remembering where to come back to.
- `ret`. Return to wherever we were called from.
- `cmp a, b`. Compare `a` and `b`, setting flags that the next jump can test.
- `jmp`, and the conditional jumps `jz` (jump if zero), `jnz` (if not zero), `jl`,
  `jge`, and friends. Go somewhere else, sometimes only if the last compare came
  out a certain way.
- `add`, `sub`, `and`, `or`, `xor`, `shl`, `shr`. Arithmetic and bit twiddling.
- `lea dst, [expr]`. Work out an address and put it in `dst`, without reading
  memory. Often used for quick multiplication too.
- `movsx` and `movzx`. Copy a smaller value into a bigger register, either
  sign-extended (`movsx`) or zero-extended (`movzx`).

That covers most of what you need to read a normal function.
