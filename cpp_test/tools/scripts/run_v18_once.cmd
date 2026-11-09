@echo off
setlocal
cd /d "%~dp0"
set "ARC_ANGEL_INGEST_URL=https://arcangecho-blhjmjym.manus.space/api/telemetry/ingest"
set "ARC_ANGEL_GTANGEL_TOKEN=ea_QayQbgxcu8m8mKdLv6JI53FIEEwQ6Cf3h-razJH6QlE"
set "ARC_ANGEL_PURPOSE_TOKEN=ea_o4nQ3zG4AbJBpZcrySN1-3Mf4qid1ZSmOBfICn84rr4"
set "ARC_ANGEL_EXPRESSION_TOKEN=ea_FF8ePMhWb4Mz0vUGNWflcnarAkxgHCzICdlAhOGAVTw"
set "GTANGEL_PEER_ID=gtangel-alpha"
start "GTAngel v18" "%CD%\gtangel_agi_v18.exe"
timeout /t 3 /nobreak >nul
endlocal
del "%~f0"
