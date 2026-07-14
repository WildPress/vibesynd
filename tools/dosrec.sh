#!/usr/bin/env bash
# dosrec.sh -- run a DOS/4GW program under DOSBox on a VIRTUAL display in the container and
# RECORD it: an mp4 (for replay/analysis) + periodic PNG frames (for the agent to read).
# Optionally drive input via a keystroke script so sessions are reproducible.
#
#   bash tools/dosrec.sh "<dos-exe>" <seconds> <out-prefix> [keyscript]
# Runs IN-CONTAINER with the repo at /work and the GOG game dir at /gog (mount both). Frames land
# at <out-prefix>_NN.png and the video at <out-prefix>.mp4 under build/.
set -u
EXE="${1:-d:\\GAMEO.EXE}"
SECS="${2:-12}"
OUT="${3:-build/rec/frame}"
KEYS="${4:-}"
mkdir -p "$(dirname "$OUT")"

# deps (xvfb virtual display, ffmpeg capture, imagemagick screenshot, xdotool input)
command -v Xvfb >/dev/null 2>&1 || {
  export DEBIAN_FRONTEND=noninteractive
  apt-get update -qq >/dev/null 2>&1
  apt-get install -y -qq xvfb ffmpeg imagemagick xdotool x11-utils >/dev/null 2>&1
}

export DISPLAY=:99
export SDL_VIDEODRIVER=x11        # render to the virtual display...
export SDL_AUDIODRIVER=dummy      # ...but no real audio device in the container (else SDL aborts)
Xvfb :99 -screen 0 1024x768x24 -nolisten tcp >/dev/null 2>&1 &
XVFB=$!
sleep 2

CONF=$(mktemp)
cat > "$CONF" <<EOF
[sdl]
output=surface
autolock=false
[dosbox]
machine=svga_s3
memsize=16
[cpu]
core=auto
cputype=auto
cycles=8000
[dos]
xms=true
ems=true
umb=true
[render]
aspect=false
[autoexec]
mount c /gog
mount d /work/run
set PATH=D:\\;C:\\
c:
$EXE > d:\\LOG.TXT
EOF

# record the whole session to mp4
ffmpeg -y -f x11grab -video_size 1024x768 -framerate 15 -i :99 -t "$SECS" \
       -pix_fmt yuv420p "${OUT}.mp4" >/dev/null 2>&1 &
FF=$!

dosbox -conf "$CONF" >"$(dirname "$OUT")/dosbox.log" 2>&1 &
DB=$!
sleep 3   # let it boot into graphics

# optional scripted keystrokes: lines of "delay <ms>" or "key <xdotool-keyname>"
if [ -n "$KEYS" ] && [ -f "$KEYS" ]; then
  while read -r cmd arg; do
    case "$cmd" in
      delay) sleep "$(awk "BEGIN{print $arg/1000}")" ;;
      key)   xdotool search --name DOSBox key --window %1 "$arg" 2>/dev/null || xdotool key "$arg" 2>/dev/null ;;
      type)  xdotool type "$arg" 2>/dev/null ;;
    esac
  done < "$KEYS"
fi

# grab PNG frames across the run for the agent to read
N=$(( SECS / 2 )); [ "$N" -lt 1 ] && N=1
for i in $(seq 1 "$N"); do
  import -window root "${OUT}_$(printf %02d "$i").png" 2>/dev/null \
    || ffmpeg -y -f x11grab -video_size 1024x768 -i :99 -frames:v 1 "${OUT}_$(printf %02d "$i").png" >/dev/null 2>&1
  sleep 2
done

kill "$DB" "$FF" "$XVFB" 2>/dev/null; wait 2>/dev/null
echo "=== program LOG.TXT ==="; cat /work/run/LOG.TXT 2>/dev/null | tr -d '\r' | head
echo "=== outputs ==="; ls -la "${OUT}"*.png "${OUT}.mp4" 2>/dev/null
