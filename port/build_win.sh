#!/bin/bash
# build_win.sh -- cross-compile the native render demo to a real Windows .exe (MinGW-w64).
#
# Requires:  gcc-mingw-w64-x86-64  (apt: sudo apt install gcc-mingw-w64-x86-64-win32)
#            SDL2 MinGW dev libs    (run build/get_sdl_mingw.sh -- fetched, no root)
# Produces:  port/build/win/syn-demo.exe  + SDL2.dll  (runs natively on Windows, no WSL/DOSBox)
set -e
cd "$(dirname "$0")/.."
CC=x86_64-w64-mingw32-gcc
command -v "$CC" >/dev/null 2>&1 || { echo "missing $CC -- install gcc-mingw-w64-x86-64-win32"; exit 1; }
# SDL2 MinGW dev libs: SYN_SDL_MINGW (e.g. the docker image's /opt/sdl2-mingw), else fetched locally
SDL="${SYN_SDL_MINGW:-build/sdl2-mingw}"
[ -f "$SDL/lib/libSDL2.dll.a" ] || bash build/get_sdl_mingw.sh
out=port/build/win
mkdir -p "$out"
"$CC" -O2 -Wall \
    -I"$SDL/include" -I"$SDL/include/SDL2" \
    port/demo_render.c port/platform_sdl.c port/gfx_soft.c port/rnc.c \
    -L"$SDL/lib" -lmingw32 -lSDL2 \
    -o "$out/syn-demo.exe"
cp "$SDL/bin/SDL2.dll" "$out/"
echo "built: $out/syn-demo.exe (+ SDL2.dll)"
file "$out/syn-demo.exe" | cut -d: -f2-
