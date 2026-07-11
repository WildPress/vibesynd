#!/usr/bin/env bash
# Compile ALL SRC*.C in a work dir to matching O*.OBJ in ONE DOSBox session,
# amortizing the ~0.85s DOSBox startup across the whole batch. The caller
# pre-populates <workdir> with SRC00.C, SRC01.C, ... (DOS 8.3 names). Used by the
# batched search harness (tools/permute_par.py --batch) to guess code blocks fast.
#   wcc95_batch.sh <workdir> "<flags>"
set -u
W="${1:?workdir}"; FLAGS="${2:--4s -oneatx -zp8 -s -zq}"
# WAT_ROOT lets the caller point mount C at a fast (native) copy of the compiler
# tree instead of the slow /mnt/c drvfs mount. Default keeps standalone use working.
WAT="${WAT_ROOT:-/work/toolchain/watcom95}"
command -v dosbox >/dev/null 2>&1 || { export DEBIAN_FRONTEND=noninteractive
  (apt-get update -qq && apt-get install -y -qq dosbox) >/dev/null 2>&1; }

# one wcc386 line per SRCnn.C -> Onn.OBJ, CRLF line endings for DOS
: > "$W/GO.BAT"
for f in "$W"/SRC*.C; do
  b=$(basename "$f" .C); n=${b#SRC}
  printf 'wcc386 %s -fo=O%s.OBJ %s.C\r\n' "$FLAGS" "$n" "$b" >> "$W/GO.BAT"
done

cat > "$W/dbx.conf" <<EOF
[cpu]
core=auto
cycles=max
[autoexec]
mount c $WAT
mount d $W
set WATCOM=C:\\
set INCLUDE=C:\\H
set DOS4G=quiet
set PATH=C:\\BIN;Z:\\
d:
call D:\\GO.BAT
exit
EOF

rm -f "$W"/O*.OBJ
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 600 dosbox -conf "$W/dbx.conf" >/dev/null 2>&1
ls "$W"/O*.OBJ >/dev/null 2>&1
