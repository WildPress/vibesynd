@echo off
REM ---------------------------------------------------------------------------
REM  Syndicate -- native port, live window.
REM  Double-click to build (first run only) and launch the game in an SDL window
REM  via WSL + WSLg. Close the window, or press Shift+Esc, to quit.
REM
REM  Needs: WSL2 with the port build tools (gcc-multilib, binutils, python3) and
REM  your own Syndicate data at the GOG path. First launch builds and takes a
REM  minute; later launches are quick.
REM ---------------------------------------------------------------------------
title Syndicate (native port)
echo Building/launching the native port window... (first run takes a minute)
echo.
wsl bash -c "cd /mnt/c/Users/James/freesynd && bash port/run_window.sh"
echo.
echo Window closed.
pause
