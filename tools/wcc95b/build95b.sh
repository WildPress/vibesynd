#!/bin/bash
command -v dosbox >/dev/null 2>&1 || { apt-get update -qq >/dev/null 2>&1; apt-get install -y -qq dosbox >/dev/null 2>&1; }
W=/tmp/b95b; rm -rf $W; mkdir -p $W
cp /work/toolchain/watcom10a/WATCOM/BINB/BPATCH.EXE $W/BPATCH.EXE
cp /work/toolchain/w95b_dl/p32/Patch32/A/WCC386D.A $W/PA.A
cp /work/toolchain/w95b_dl/p32/Patch32/B/WCC386D.B $W/PB.B
cp /work/toolchain/watcom95/BIN/WCC386.EXE $W/WCC386.EXE
printf 'path=d:\\r\nbpatch -p PA.A > R.LOG\r\nbpatch -p PB.B >> R.LOG\r\nbpatch -q WCC386.EXE >> R.LOG\r\n' > $W/GO.BAT
cat > $W/dbx.conf <<CONF
[cpu]
core=auto
cycles=max
[autoexec]
mount d $W
d:
call D:\GO.BAT
exit
CONF
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 120 dosbox -conf $W/dbx.conf >/dev/null 2>&1 || true
echo "=== files in workdir ==="; ls -la $W/ | grep -iE "wcc386|\.bak"
echo "=== R.LOG ==="; cat $W/R.LOG 2>/dev/null
# grab the patched file (case-insensitive)
PATCHED=$(ls $W/ | grep -i "^wcc386.exe$" | head -1)
if [ -n "$PATCHED" ]; then
  sz=$(wc -c < "$W/$PATCHED"); md=$(md5sum "$W/$PATCHED"|cut -d' ' -f1)
  echo "patched wcc386: size=$sz md5=$md"
  mkdir -p /work/toolchain/watcom95b/BIN
  cp "$W/$PATCHED" /work/toolchain/watcom95b/BIN/WCC386.EXE
  cp /work/toolchain/watcom95/BIN/DOS4GW.EXE /work/toolchain/watcom95b/BIN/ 2>/dev/null
  [ -d /work/toolchain/watcom95/unpacked ] && mkdir -p /work/toolchain/watcom95b/H && cp -r /work/toolchain/watcom95/H/* /work/toolchain/watcom95b/H/ 2>/dev/null
  echo "SAVED toolchain/watcom95b/BIN/WCC386.EXE"
fi
