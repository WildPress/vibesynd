#!/bin/bash
# build_demo.sh -- build the native render demo (port/build/syn-demo).
#
# Prefers a system SDL2 dev install (pkg-config / sdl2-config). If neither is present
# (e.g. a WSL with only the runtime lib), it fetches just the SDL2 headers into
# build/sdl2/include via `apt-get download` (no root needed) and links the runtime .so
# directly. A genuine Windows .exe is a follow-up (needs a Windows SDL2 toolchain).
set -e
cd "$(dirname "$0")/.."
mkdir -p port/build

SRC="port/demo_render.c port/platform_sdl.c port/gfx_soft.c port/rnc.c"
CFLAGS="-O2 -Wall -D_REENTRANT"

if pkg-config --exists sdl2 2>/dev/null; then
    INC=$(pkg-config --cflags sdl2); LIB=$(pkg-config --libs sdl2)
elif command -v sdl2-config >/dev/null 2>&1; then
    INC=$(sdl2-config --cflags); LIB=$(sdl2-config --libs)
else
    echo "no SDL2 dev install; fetching SDL2 headers into build/sdl2/include (no root)"
    base="build/sdl2/include"
    if [ ! -f "$base/SDL2/SDL.h" ]; then
        work=$(mktemp -d); chmod 777 "$work"
        ( cd "$work" && apt-get download libsdl2-dev >/dev/null 2>&1 \
          && dpkg -x ./*.deb root )
        mkdir -p "$base/SDL2"
        cp -r "$work"/root/usr/include/SDL2/. "$base/SDL2/"
        find "$work"/root/usr/include -name "_real_SDL_config.h" -exec cp {} "$base/SDL2/" \;
        rm -rf "$work"
    fi
    INC="-I$base -I$base/SDL2"
    LIB="$(ls /usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0 | head -1) -lm -lpthread -ldl"
fi

# shellcheck disable=SC2086
gcc $CFLAGS $INC $SRC $LIB -o port/build/syn-demo
echo "built: port/build/syn-demo"
