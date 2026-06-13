@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
set "SDP_FILE=%SCRIPT_DIR%video.sdp"

ffplay ^
  -protocol_whitelist file,udp,rtp ^
  -fflags nobuffer ^
  -flags low_delay ^
  -framedrop ^
  -avioflags direct ^
  -probesize 32 ^
  -analyzeduration 0 ^
  -sync video ^
  "%SDP_FILE%"

