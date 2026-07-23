# Syndicate native-port build image.
#
# A reproducible environment for building the `port` branch: the 32-bit blob + shim toolchain,
# the MinGW cross-compiler for a native Windows .exe, and both 64- and 32-bit SDL2. The game
# binary and data are NEVER baked in -- mount the repo (and, to run, your own data) at runtime.
#
# Build:  docker build -t synd-port -f docker/port.Dockerfile .
# Use:    docker run --rm -v "$PWD":/work synd-port bash docker/port-build.sh
#         (add -v "<your DATA dir>":/gog:ro to also run the headless boot render)
#
# The 32-bit blob + all the shim/emulator C is Linux ELF; run those under WSL/Linux. The Windows
# .exe (render demo) runs on Windows directly. A live SDL *window* from a container needs X11/
# Wayland forwarding to the host -- for that, run the built binary under WSLg instead.

FROM debian:bookworm-slim

# i386 arch is needed for the 32-bit SDL2 runtime the game (32-bit) links against.
RUN dpkg --add-architecture i386 \
 && apt-get update \
 && apt-get install -y --no-install-recommends \
      ca-certificates curl xz-utils tar \
      python3 \
      build-essential gcc-multilib g++-multilib binutils \
      gcc-mingw-w64-x86-64-win32 gcc-mingw-w64-i686-win32 \
      libsdl2-dev libsdl2-dev:i386 \
 && rm -rf /var/lib/apt/lists/*

# SDL2's prebuilt MinGW dev libraries (headers + import lib + SDL2.dll) for the Windows .exe --
# both the 64-bit (render demo) and 32-bit (the native game .exe) arches.
ARG SDL_VER=2.30.11
RUN curl -fsSL "https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VER}/SDL2-devel-${SDL_VER}-mingw.tar.gz" \
      | tar -xz -C /tmp \
 && mkdir -p /opt/sdl2-mingw /opt/sdl2-mingw32 \
 && cp -r "/tmp/SDL2-${SDL_VER}/x86_64-w64-mingw32/." /opt/sdl2-mingw/ \
 && cp -r "/tmp/SDL2-${SDL_VER}/i686-w64-mingw32/."   /opt/sdl2-mingw32/ \
 && rm -rf "/tmp/SDL2-${SDL_VER}"
ENV SYN_SDL_MINGW=/opt/sdl2-mingw
ENV SYN_SDL_MINGW32=/opt/sdl2-mingw32

WORKDIR /work
