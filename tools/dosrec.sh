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

# deps (xvfb virtual display, ffmpeg capture, imagemagick screenshot, xdotool input, twm WM).
# twm matters: SDL 1.2 (DOSBox) only accepts REAL input when its window holds the X input focus,
# and with no window manager on Xvfb nothing ever grants focus, so xdotool keys/clicks go nowhere.
# twm's default focus model is focus-follows-mouse, so parking the pointer over the DOSBox window
# gives it focus and XTEST events land. (Isolation test: at a DOS prompt, `type dir` did nothing
# WITHOUT twm and echoed correctly WITH it.)
command -v Xvfb >/dev/null 2>&1 && command -v twm >/dev/null 2>&1 || {
  export DEBIAN_FRONTEND=noninteractive
  apt-get update -qq >/dev/null 2>&1
  apt-get install -y -qq xvfb ffmpeg imagemagick xdotool x11-utils twm >/dev/null 2>&1
}

export DISPLAY=:99
export SDL_VIDEODRIVER=x11        # render to the virtual display...
export SDL_AUDIODRIVER=dummy      # ...but no real audio device in the container (else SDL aborts)
Xvfb :99 -screen 0 1024x768x24 -nolisten tcp >/dev/null 2>&1 &
XVFB=$!
sleep 2
# minimal WM for focus-follows-mouse (no titlebars/borders so the 640x400 render sits at 0,0)
if command -v twm >/dev/null 2>&1; then
  printf 'NoTitle\nNoGrabServer\nRandomPlacement\nDecorateTransients\n' > /tmp/.twmrc
  HOME=/tmp twm -f /tmp/.twmrc >/dev/null 2>&1 &
  WM=$!
  sleep 1
fi

CONF=$(mktemp)
cat > "$CONF" <<EOF
[sdl]
output=surface
autolock=false
[dosbox]
machine=svga_s3
memsize=16
[cpu]
core=${DOSREC_CORE:-auto}
cputype=auto
cycles=${DOSREC_CYCLES:-8000}
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
${DOSREC_CD:+cd $DOSREC_CD}
$EXE > d:\\LOG.TXT
EOF

# record the whole session to mp4
ffmpeg -y -f x11grab -video_size 1024x768 -framerate 15 -i :99 -t "$SECS" \
       -pix_fmt yuv420p "${OUT}.mp4" >/dev/null 2>&1 &
FF=$!

# DOSREC_DBG overrides the emulator binary (e.g. build/dosbox-dbg for tracing); the
# trace env (TRACEOUT/TRACEUNIQ) is inherited and the tracer's stderr markers land in
# the dosbox.log next to the output.
"${DOSREC_DBG:-dosbox}" -conf "$CONF" >"$(dirname "$OUT")/dosbox.log" 2>&1 &
DB=$!
sleep 3   # let it boot into graphics
# pin the DOSBox window to the top-left so the 640x400 render sits at 0,0 (twm RandomPlacement
# otherwise scatters it, breaking the game-pixel*2 click coords and the frame layout).
for _ in 1 2 3; do
  _wid=$(xdotool search --name "DOSBox" 2>/dev/null | tail -1)
  [ -n "$_wid" ] && { xdotool windowmove "$_wid" 0 0 2>/dev/null; xdotool windowraise "$_wid" 2>/dev/null; break; }
  sleep 1
done

# optional scripted input. One command per line:
#   delay <ms>          wait
#   key   <xdotool-key> send a keystroke (e.g. Return, space, Escape)
#   type  <text>        type a string
#   move  <X> <Y>       move the mouse to absolute display coords
#   click [button]      click (button defaults to 1 = left)
# The game renders 320x200 scaled 2x into a window at the top-left of the 1024x768
# display, so display coord ~= game-pixel * 2. Calibrate against the captured frames.
#
# INPUT MUST BE REAL (XTEST), NOT SYNTHETIC. DOSBox is an SDL 1.2 app and ignores XSendEvent
# (`xdotool ... --window`). twm (started above) provides focus-follows-mouse, so we just keep the
# pointer parked over the DOSBox window and drive it with plain xdotool key/click (XTEST) which
# deliver genuine events to the focused window.
#
# The input loop runs in the BACKGROUND, concurrently with the frame-capture loop below, so a long
# keyscript no longer delays or starves the PNG grabs (they must span the whole session).
drive_input() {
  local WID cmd arg
  WID=$(xdotool search --name "DOSBox" 2>/dev/null | tail -1)
  echo "input: DOSBox window id=${WID:-<none found>}" >&2
  [ -n "$WID" ] && xdotool windowraise "$WID" 2>/dev/null
  xdotool mousemove 320 200 2>/dev/null
  while read -r cmd arg; do
    xdotool mousemove 320 200 2>/dev/null              # keep pointer over the window (focus-follows-mouse)
    case "$cmd" in
      delay) sleep "$(awk "BEGIN{print $arg/1000}")" ;;
      key)   xdotool key --clearmodifiers "$arg" 2>/dev/null ;;
      type)  xdotool type --clearmodifiers "$arg" 2>/dev/null ;;
      move)  xdotool mousemove $arg 2>/dev/null ;;
      click) xdotool click "${arg:-1}" 2>/dev/null ;;
      down)  xdotool mousedown "${arg:-1}" 2>/dev/null ;;
      up)    xdotool mouseup "${arg:-1}" 2>/dev/null ;;
    esac
  done < "$KEYS"
}
if [ -n "$KEYS" ] && [ -f "$KEYS" ]; then
  drive_input &
  KEYPID=$!
fi

# grab PNG frames across the run for the agent to read (concurrent with input)
N=$(( SECS / 2 )); [ "$N" -lt 1 ] && N=1
for i in $(seq 1 "$N"); do
  import -window root "${OUT}_$(printf %02d "$i").png" 2>/dev/null \
    || ffmpeg -y -f x11grab -video_size 1024x768 -i :99 -frames:v 1 "${OUT}_$(printf %02d "$i").png" >/dev/null 2>&1
  sleep 2
done
kill "${KEYPID:-}" 2>/dev/null

kill "$DB" "$FF" "$XVFB" ${WM:-} 2>/dev/null; wait 2>/dev/null
echo "=== program LOG.TXT ==="; cat /work/run/LOG.TXT 2>/dev/null | tr -d '\r' | head
echo "=== outputs ==="; ls -la "${OUT}"*.png "${OUT}.mp4" 2>/dev/null
