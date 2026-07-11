#!/usr/bin/env bash
# Compile an ARBITRARY source file to an ARBITRARY obj with Watcom 9.5b, using a
# caller-supplied DOSBox work dir so many instances can run in parallel without
# clashing on the shared SRC.C/SRC.OBJ. Used by tools/permute_par.py.
#   wcc95_file.sh <src.c> <out.obj> <workdir> "<flags>"
set -u
src="${1:?src}"; out="${2:?out}"; WORK="${3:?workdir}"; FLAGS="${4:--4s -oneatx -zp8 -s -zq}"
ROOT=/work/toolchain
command -v dosbox >/dev/null 2>&1 || { export DEBIAN_FRONTEND=noninteractive
  (apt-get update -qq && apt-get install -y -qq dosbox) >/dev/null 2>&1; }
mkdir -p "$WORK"
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
[ -f "$WORK/SRC.OBJ" ] && cp "$WORK/SRC.OBJ" "$out" || exit 1
