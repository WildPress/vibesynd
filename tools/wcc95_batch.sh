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

# WCC_DOSEMU=1 -> compile the batch under dosemu2/KVM instead of DOSBox: the SAME wcc386 binary runs on
# the real CPU (KVM), producing byte-identical _TEXT+fixups (validated) at ~2.5x DOSBox throughput and far
# less host CPU per instance, so it scales under heavy parallelism where DOSBox contends. Needs the
# dosemu2 image + --device /dev/kvm. Falls through to DOSBox otherwise.
if [ "${WCC_DOSEMU:-0}" = "1" ] && command -v dosemu >/dev/null 2>&1; then
  export HOME=/root
  ID=$(echo "$W" | md5sum | cut -c1-6 | tr 'a-z' 'A-Z')      # 8.3-safe unique batch name per workdir
  GO="/root/.dosemu/drive_c/G$ID.BAT"
  { printf '@echo off\r\n'
    printf 'lredir g: linux\\fs%s\r\n' "$WAT"
    printf 'lredir h: linux\\fs%s\r\n' "$W"
    printf 'set WATCOM=G:\\\r\n'; printf 'set DOS4G=quiet\r\n'; printf 'set PATH=G:\\BIN;Z:\\bin\r\n'
    printf 'h:\r\n'
    for f in "$W"/SRC*.C; do b=$(basename "$f" .C); n=${b#SRC}
      printf 'g:\\BIN\\WCC386.EXE %s -fo=O%s.OBJ %s.C\r\n' "$FLAGS" "$n" "$b"; done
    printf 'exitemu\r\n'
  } > "$GO"
  rm -f "$W"/O*.OBJ "$W"/o*.obj
  timeout 600 dosemu -dumb -quiet -E "c:\\G$ID.BAT" >/dev/null 2>&1
  # the linux redirector writes lowercase names; normalise to the O*.OBJ the caller globs for
  for f in "$W"/o*.obj; do [ -e "$f" ] && mv "$f" "$W/$(basename "$f" | tr 'a-z' 'A-Z')"; done 2>/dev/null
  rm -f "$GO"
  ls "$W"/O*.OBJ >/dev/null 2>&1
  exit 0
fi

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
