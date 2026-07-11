#!/usr/bin/env bash
# Decompress a WPACK-compressed Watcom 9.5b distribution file using WPACK.EXE
# (from the 10.0a ISO) under DOSBox.  Usage: wunpack95.sh <PACKEDFILE> [more...]
# Files land in toolchain/watcom95/unpacked/.
set -u
ROOT=/work/toolchain
SRCDIR="$ROOT/w95_dl/Watcom CPP 9.5b (1993) (3.5-1.44mb)"
OUT="$ROOT/watcom95/unpacked"; mkdir -p "$OUT"
WORK="$ROOT/doswork_95"; rm -rf "$WORK"; mkdir -p "$WORK"

command -v dosbox >/dev/null 2>&1 || { export DEBIAN_FRONTEND=noninteractive
  (apt-get update -qq && apt-get install -y -qq dosbox p7zip-full) >/dev/null 2>&1; }

cp "$ROOT/watcom10a_iso/WPACK.EXE" "$WORK/WPACK.EXE"
cp "$ROOT/watcom95/BIN/DOS4GW.EXE" "$WORK/DOS4GW.EXE" 2>/dev/null || true

# gather the packed files: extract each named file from whichever floppy holds it
cd "$SRCDIR"
{ echo '@echo off'; } > "$WORK/GO.BAT"
for f in "$@"; do
  # find which W9532 image contains it
  for img in W9532_*.img; do
    if 7z l "$img" 2>/dev/null | grep -qi " $f\$"; then
      7z e -y "$img" "$f" -o"$WORK" >/dev/null 2>&1 && echo "  got $f from $img" >&2
      break
    fi
  done
  echo "WPACK $f" >> "$WORK/GO.BAT"
done
echo 'exit' >> "$WORK/GO.BAT"

cat > "$WORK/dbx.conf" <<EOF
[cpu]
core=auto
cycles=max
[autoexec]
mount d $WORK
d:
call D:\\GO.BAT
exit
EOF

SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 120 dosbox -conf "$WORK/dbx.conf" >/dev/null 2>&1
echo "=== unpacked files in $WORK ==="
ls -la "$WORK"
# copy anything that isn't our tooling/input back to OUT
for x in "$WORK"/*; do
  b=$(basename "$x")
  case "$b" in WPACK.EXE|DOS4GW.EXE|GO.BAT|dbx.conf) continue;; esac
  cp "$x" "$OUT/" 2>/dev/null || true
done
echo "=== OUT ($OUT) ==="; ls -la "$OUT"
