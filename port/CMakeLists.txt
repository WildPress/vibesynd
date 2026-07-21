# Native port build (branch: port). See port/README.md and ../BRANCHES.md.
#
# Compiles the portable game-logic C from ../src together with a platform backend
# (stub for now, SDL2 later) into a native executable. Byte-matching is NOT a goal here.
#
#   cmake -S port -B build-native && cmake --build build-native
#
# NOTE: this is the scaffold. It does not fully link yet -- the game logic references
# globals (to be generated from manifest/globals.json) and the src/lib platform routines
# (to be routed through platform.h). Bring-up is staged in port/README.md.

cmake_minimum_required(VERSION 3.16)
project(syndicate_native C)

set(CMAKE_C_STANDARD 99)
# faithful data layout: build 32-bit so struct/pointer sizes match the data files
option(SYND_M32 "build 32-bit to match original data layout" ON)
if(SYND_M32)
  add_compile_options(-m32)
  add_link_options(-m32)
endif()

# Portable game logic: every src/**/*.c that is NOT the src/lib assembly layer and NOT a
# db-byte transcription. (CMake can't inspect contents, so the concrete file list is
# produced by tools/port_sources.py during bring-up; globbed here as a placeholder.)
file(GLOB_RECURSE GAME_LOGIC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/../src/*.c)
list(FILTER GAME_LOGIC EXCLUDE REGEX "/src/lib/")

add_executable(syndicate
  ${GAME_LOGIC}
  ${CMAKE_SOURCE_DIR}/platform_stub.c
  # ${CMAKE_SOURCE_DIR}/globals.c        # generated from manifest/globals.json (bring-up step 1)
  # ${CMAKE_SOURCE_DIR}/main.c           # entry: init platform, call the game main loop
)
target_include_directories(syndicate PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_options(syndicate PRIVATE -w)   # silence during bring-up; tighten later

# SDL2 backend (later): find_package(SDL2); target_link_libraries(syndicate SDL2::SDL2)
