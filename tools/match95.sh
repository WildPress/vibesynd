#!/usr/bin/env bash
# Match one function with the PERIOD Watcom 9.5b compiler + relocation-aware diff.
#   docker run --rm -v "$PWD":/work -w /work synd-decomp bash tools/match95.sh <name> ["<flags>"]
set -u
name="${1:?usage: match95.sh <name> [flags]}"
FLAGS="${2:--4s -oneatx -zp8 -s -zq}"
if ! bash tools/wcc_95.sh "$name" "$FLAGS" >/tmp/wcc95.log 2>&1; then
  echo "COMPILE FAILED for $name (flags: $FLAGS)"; tail -5 /tmp/wcc95.log; exit 1
fi
SKIP_COMPILE=1 python3 tools/match_reloc.py "$name"
