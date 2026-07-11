#!/usr/bin/env bash
# Match one function with the PERIOD Watcom 10.0a compiler + relocation-aware diff.
# This is the loop primitive: writes src/<name>.c yourself first, then:
#   docker run --rm -v "$PWD":/work -w /work synd-decomp bash tools/match10.sh <name> ["<flags>"]
# Default flags target stack-calling functions; pass "-5r ..." for register-calling.
set -u
name="${1:?usage: match10.sh <name> [flags]}"
FLAGS="${2:--4s -oneatx -zp8 -s -zq}"
if ! bash tools/wcc_dos.sh "$name" "$FLAGS" >/tmp/wcc.log 2>&1; then
  echo "COMPILE FAILED for $name (flags: $FLAGS)"; tail -5 /tmp/wcc.log; exit 1
fi
SKIP_COMPILE=1 python3 tools/match_reloc.py "$name"
