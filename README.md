# Syndicate matching decompilation

<!--PROGRESS--><a href="https://github.com/WildPress/vibesynd/wiki"><img alt="game matched" src="https://img.shields.io/badge/game%20matched-61.83%25%20bytes%20%7C%20403%2F514%20fns-blue"></a><!--/PROGRESS--> <a href="docs/journal.md"><img alt="runtime" src="https://img.shields.io/badge/runs-boot%20%E2%86%92%20menu%20%E2%86%92%20world%20map-2ea043"></a> <a href="docs/journal.md"><img alt="sound" src="https://img.shields.io/badge/sound-plays%20(matches%20original)-2ea043"></a>

A matching decompilation of the original 1993/95 DOS *Syndicate*: C source that,
compiled with the period Watcom toolchain, reproduces the game's machine code byte
for byte, one function at a time. A project to learn how decompilation works.

## Progress at a glance

![Decompilation progress treemap: functions sized by code bytes, coloured by match status](docs/treemap.svg)

Every function in the code segment, area proportional to its size, grouped by subsystem.
**Green** = our C compiles byte-identical to the original. **Blue** = complete, differing only in a
register the compiler happened to choose. **Amber** = decoded to readable C, but the period Watcom
compiler will not reproduce these exact bytes from any source spelling. Amber is a codegen ceiling,
proven in [register allocation](docs/register-allocation.md), **not unfinished work**: those functions
are understood and written, they simply cannot go green from C. So green is the byte-match score and
amber is "done as far as the compiler allows". Regenerate with `python tools/treemap.py`; a live,
hover-able version and a matched-over-time chart are in `tools/progress.py` (local `dashboard/progress.html`).

## The reconstruction runs

The treemap measures one axis, byte-matching. There is a second axis it does not show: whether the
rebuilt executable actually *runs*. It does. Built from the original bytes with the fixups reconstructed,
it boots under DOS/4GW, plays the Bullfrog intro with subtitles, reaches the main menu and the world map
with full dashboard text, and **plays sound** whose captured output matches the original's to a fraction
of a decibel. The full story is in the [journal](docs/journal.md); the most recent entry is the sound
fix, a relocation the rebuild had been emitting across a record boundary and so corrupting.

## Start with the wiki

The **[project wiki](https://github.com/WildPress/vibesynd/wiki)** explains
everything in plain language: what a matching decompilation is, the DOS and Watcom
toolchain, the core concepts (stack frames, calling conventions, relocations and
OMF, compiler flags, register allocation), a running journal of the work, and a
growing map of what the game's code actually does. The wiki is generated from the
[`docs/`](docs/) folder in this repo, so it lives here too.

## Legal and hygiene

This works only from static and dynamic analysis of a binary you own. No original
source is used. The copyrighted game binary and extracted data (`inputs/`), the
Ghidra databases (`ghidra/`), and the abandonware Watcom toolchain (`toolchain/`)
are git-ignored and never committed. To use it you must supply your own copy of the
game. *Syndicate* and Watcom are © their respective owners; this project is
independent and unaffiliated.

## Layout

```
docs/            the wiki source: explainers, journal, game notes
src/             our written, matched C
tools/           the matching harness and analysis scripts
manifest/        functions.json (status), library map, recipes
ghidra_scripts/  headless inventory scripts
docker/          the pipeline image
inputs/ ghidra/ toolchain/ build/    git-ignored
```
