#!/usr/bin/env bash
# Host-side wrapper (run from WSL): execute a command inside the synd-decomp
# pipeline image with this decomp repo mounted at /work.
#
#   ./run.sh                      # interactive shell in the container
#   ./run.sh bash tools/smoke.sh  # run the compile+diff smoke test
#   ./run.sh python3 tools/match.py <function>
#
# The original binary lives under inputs/ (git-ignored) and is mounted with the
# rest of the repo, read-only from the container's point of view for safety.
set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMAGE="${SYND_IMAGE:-synd-decomp}"

TTY_FLAGS=""
if [ -t 0 ] && [ -t 1 ]; then TTY_FLAGS="-it"; fi

exec docker run --rm $TTY_FLAGS \
  -v "$REPO_DIR":/work \
  -w /work \
  "$IMAGE" "$@"
