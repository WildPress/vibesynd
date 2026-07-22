#!/bin/bash
# run_window.sh -- launch the interactive native port (game process + SDL viewer window).
# Needs a display (WSLg provides one). Close the window, or Shift+Esc, to quit.
set -e
cd "$(dirname "$0")/.."
b=port/gen/blob
bash port/build_window.sh    # always rebuild (~10s) so a launch never runs stale binaries
# clear any stale shared-memory segment
rm -f /dev/shm/syndicate_port_shm 2>/dev/null || true
( cd build/rundir && SYN_NODLL=1 exec ../../"$b/game_boot" ) &
game=$!
sleep 0.5
"$b/viewer"
kill "$game" 2>/dev/null || true
rm -f /dev/shm/syndicate_port_shm 2>/dev/null || true
