#!/bin/bash
command -v dosbox >/dev/null 2>&1 || { apt-get update -qq >/dev/null 2>&1; apt-get install -y -qq dosbox >/dev/null 2>&1; }
name="$1"; flags="${2:--4s -oneatx -zp8 -s -zq}"
W=$(mktemp -d); ROOT=/work/toolchain/watcom95b
cp "/work/src/${name}.c" "$W/SRC.C"
printf 'wcc386 %s -fo=SRC.OBJ SRC.C > BUILD.LOG\r\n' "$flags" > "$W/GO.BAT"
cat > "$W/dbx.conf" <<CONF
[cpu]
core=auto
cycles=max
[autoexec]
mount c $ROOT
mount d $W
set WATCOM=C:\
set INCLUDE=C:\H
set DOS4G=quiet
set PATH=C:\BIN;Z:\
d:
call D:\GO.BAT
exit
CONF
rm -f "$W/SRC.OBJ"
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 90 dosbox -conf "$W/dbx.conf" >/dev/null 2>&1
if [ -f "$W/SRC.OBJ" ]; then mkdir -p /work/build; cp "$W/SRC.OBJ" "/work/build/${name}.obj"; echo "compiled ${name} (9.5b)"; else echo "COMPILE FAILED"; cat "$W/BUILD.LOG" 2>/dev/null; fi
rm -rf "$W"
