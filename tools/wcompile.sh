#!/usr/bin/env bash
# Compile one decomp unit with Open Watcom, targeting 32-bit DOS/4GW like the
# original 1995 build. Emits build/<name>.obj (Watcom OMF).
#
# Usage: bash tools/wcompile.sh <name>        # compiles src/<name>.c
#
# WATFLAGS holds the current best-guess flags. These are TUNABLE and will be
# refined once we diff against the real binary -- CPU level, calling convention,
# optimization bundle, and packing all affect codegen and thus the match.
set -euo pipefail

name="${1:?usage: wcompile.sh <name>}"
src="src/${name}.c"
[ -f "$src" ] || { echo "no such source: $src" >&2; exit 1; }

mkdir -p build

# -5r    : Pentium scheduling, register calling convention.
#          NOTE: the original was built with Watcom 10.0-family, which has NO -6
#          CPU level -> the game uses at most -5 (Pentium). Use -5r, not -6r.
#          (-5 vs -4 vs -3 still TBD; pin it on a scheduling-sensitive function.)
# -oneatx: optimize (numeric, expand-intrinsics, alias-relax, time, all)
# -zp8   : 8-byte struct packing
# -s     : no stack-overflow checks (release games disabled these)
# -zq    : quiet
: "${WATFLAGS:=-5r -oneatx -zp8 -s -zq}"

echo "wcc386 $WATFLAGS -fo=build/${name}.obj $src"
wcc386 $WATFLAGS -fo="build/${name}.obj" "$src"
echo "-> build/${name}.obj"
