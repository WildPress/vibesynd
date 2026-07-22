#!/bin/bash
# port-build.sh -- build the native-port artifacts inside the synd-port image.
#
#   docker run --rm -v "$PWD":/work synd-port bash docker/port-build.sh
#   docker run --rm -v "$PWD":/work -v "<DATA>":/gog:ro synd-port bash docker/port-build.sh
#
# Builds: the whole-game native link (blob + data), the Windows .exe (render demo), and -- if a
# data directory is mounted at /gog -- the headless boot render (the Syndicate title screen to
# build/rundir/frame.ppm). Needs the repo (incl. inputs/) mounted at /work.
set -e
cd "$(dirname "$0")/.."

# the code image is derived from the mounted original binary (never baked into the image)
[ -f build/obj1_full.bin ] || python3 tools/linearize.py

echo "===================================================================="
echo " 1/3  whole game links natively (symbolized asm blob + DGROUP data)"
echo "===================================================================="
bash port/build_asm_all.sh

echo "===================================================================="
echo " 2/3  Windows .exe (render demo, MinGW + SDL2)"
echo "===================================================================="
bash port/build_win.sh

echo "===================================================================="
echo " 3/3  boot render"
echo "===================================================================="
if [ -d /gog ]; then
    SYN_DATA=/gog SYN_TIMEOUT="${SYN_TIMEOUT:-12}" bash port/build_boot.sh || true
    if [ -f build/rundir/frame.ppm ]; then
        echo "rendered -> build/rundir/frame.ppm"
    fi
else
    echo "(no data mounted at /gog -- skipping the boot render; mount it to render the title screen)"
fi
echo "done."
