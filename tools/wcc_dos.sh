#!/usr/bin/env bash
# Compile one unit with the PERIOD Watcom 10.0a compiler (DOS-hosted WCC386.EXE,
# loaded via W32RUN) running headless under DOSBox inside the container.
#
#   bash tools/wcc_dos.sh <name> ["<wcc386 flags>"]     # src/<name>.c -> build/<name>.obj
#
# Requires the Watcom 10.0a tree staged at toolchain/watcom10a/WATCOM (git-ignored).
# NOTE: Watcom 10.0 has NO -6 CPU level; use -5r/-4r/-3r. Default below is -5r.
set -u
name="${1:?usage: wcc_dos.sh <name> [flags]}"
FLAGS="${2:--5r -oneatx -zp8 -s -zq}"
ROOT=/work/toolchain
src="src/${name}.c"
[ -f "$src" ] || { echo "no such source: $src" >&2; exit 1; }

command -v dosbox >/dev/null 2>&1 || { export DEBIAN_FRONTEND=noninteractive
  (apt-get update -qq && apt-get install -y -qq dosbox) >/dev/null 2>&1; }

WORK="$ROOT/doswork"; mkdir -p "$WORK"
cp "$src" "$WORK/SRC.C"                      # 8.3 short name for DOS
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
set PATH=C:\\WATCOM\\BINB;C:\\WATCOM\\BIN;Z:\\
d:
call D:\\GO.BAT
exit
EOF

rm -f "$WORK/SRC.OBJ"
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 120 dosbox -conf "$WORK/dbx.conf" >/dev/null 2>&1
if [ -f "$WORK/SRC.OBJ" ]; then
  mkdir -p build && cp "$WORK/SRC.OBJ" "build/${name}.obj"
  echo "-> build/${name}.obj (Watcom 10.0a, flags: $FLAGS)"
else
  echo "COMPILE FAILED (no OBJ). log:" >&2
  sed 's/^/  /' "$WORK/BUILD.LOG" 2>/dev/null >&2
  exit 1
fi
