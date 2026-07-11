#!/usr/bin/env bash
# Compile one unit with the Watcom C/C++ 10.0 LIMITED-AVAILABILITY compiler
# (Mar-1994; copyright banner matches the game). Uses the LA WCC386.EXE/W32RUN.EXE
# from toolchain/la_stage, but borrows H/ headers + DOS4GW from the watcom10a tree.
#   bash tools/wcc_la.sh <name> ["<flags>"]     # src/<name>.c -> build/<name>.obj
set -u
name="${1:?usage: wcc_la.sh <name> [flags]}"
FLAGS="${2:--3s -of -oneatx -zp8 -s -zq}"
ROOT=/work/toolchain
src="src/${name}.c"
[ -f "$src" ] || { echo "no such source: $src" >&2; exit 1; }

command -v dosbox >/dev/null 2>&1 || { export DEBIAN_FRONTEND=noninteractive
  (apt-get update -qq && apt-get install -y -qq dosbox) >/dev/null 2>&1; }

WORK="$ROOT/doswork_la"; mkdir -p "$WORK"
cp "$src" "$WORK/SRC.C"
# overlay the LA compiler binaries so they win on PATH (E:)
cp "$ROOT/la_stage/WCC386.EXE" "$WORK/WCC386.EXE"
cp "$ROOT/la_stage/W32RUN.EXE" "$WORK/W32RUN.EXE"
printf 'wcc386 %s -fo=SRC.OBJ SRC.C > BUILD.LOG\r\n' "$FLAGS" > "$WORK/GO.BAT"
cat > "$WORK/dbx.conf" <<EOF
[cpu]
core=auto
cycles=max
[autoexec]
mount c $ROOT/watcom10a
mount d $WORK
set WATCOM=C:\\WATCOM
set INCLUDE=C:\\WATCOM\\H
set PATH=D:\\;C:\\WATCOM\\BIN;Z:\\
d:
call D:\\GO.BAT
exit
EOF

rm -f "$WORK/SRC.OBJ"
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 120 dosbox -conf "$WORK/dbx.conf" >/dev/null 2>&1
if [ -f "$WORK/SRC.OBJ" ]; then
  mkdir -p build && cp "$WORK/SRC.OBJ" "build/${name}.obj"
  echo "-> build/${name}.obj (Watcom 10.0 LA, flags: $FLAGS)"
else
  echo "COMPILE FAILED (no OBJ). log:" >&2
  sed 's/^/  /' "$WORK/BUILD.LOG" 2>/dev/null >&2
  exit 1
fi
