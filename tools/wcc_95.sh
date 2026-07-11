#!/usr/bin/env bash
# Compile one unit with the period Watcom C/C++ 9.5b compiler (DOS-hosted
# WCC386.EXE, DOS/4GW-extended) running headless under DOSBox in the container.
#   bash tools/wcc_95.sh <name> ["<wcc386 flags>"]     # src/<name>.c -> build/<name>.obj
# Tree: toolchain/watcom95/BIN (WCC386.EXE + DOS4GW.EXE), git-ignored.
# NOTE: 9.5 has no -6/-5? use -4/-3. Our match sources have no #include, so no headers needed.
set -u
name="${1:?usage: wcc_95.sh <name> [flags]}"
FLAGS="${2:--4s -oneatx -zp8 -s -zq}"
ROOT=/work/toolchain
src="src/${name}.c"
[ -f "$src" ] || { echo "no such source: $src" >&2; exit 1; }

command -v dosbox >/dev/null 2>&1 || { export DEBIAN_FRONTEND=noninteractive
  (apt-get update -qq && apt-get install -y -qq dosbox) >/dev/null 2>&1; }

WORK="$ROOT/doswork_95c"; mkdir -p "$WORK"
cp "$src" "$WORK/SRC.C"
printf 'wcc386 %s -fo=SRC.OBJ SRC.C > BUILD.LOG\r\n' "$FLAGS" > "$WORK/GO.BAT"
cat > "$WORK/dbx.conf" <<EOF
[cpu]
core=auto
cycles=max
[autoexec]
mount c $ROOT/watcom95
mount d $WORK
set WATCOM=C:\\
set INCLUDE=C:\\H
set DOS4G=quiet
set PATH=C:\\BIN;Z:\\
d:
call D:\\GO.BAT
exit
EOF

rm -f "$WORK/SRC.OBJ"
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 120 dosbox -conf "$WORK/dbx.conf" >/dev/null 2>&1
if [ -f "$WORK/SRC.OBJ" ]; then
  mkdir -p build && cp "$WORK/SRC.OBJ" "build/${name}.obj"
  echo "-> build/${name}.obj (Watcom 9.5b, flags: $FLAGS)"
else
  echo "COMPILE FAILED (no OBJ). log:" >&2
  sed 's/^/  /' "$WORK/BUILD.LOG" 2>/dev/null >&2
  exit 1
fi
