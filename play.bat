@echo off
rem ============================================================
rem  Starts the chess server and opens it in Chrome.
rem  Close this window, or press Ctrl+C, to stop the server.
rem ============================================================

rem Work from the folder this file sits in, wherever it is called from.
cd /d "%~dp0"

rem Build it the first time, or after the code changes.
if not exist chess-web.exe (
    echo Building, this only happens once...
    call build-web.bat
)

rem Stop an older copy still holding the port, so this can be run twice.
taskkill /F /IM chess-web.exe >nul 2>&1

echo Starting the chess server...
echo Opening Chrome at http://localhost:8080
echo Close this window, or press Ctrl+C, to stop.
echo.

rem Wait about two seconds, then open Chrome. A short ping is used as the
rem wait because the timeout command refuses to run in some windows.
start "" /b cmd /c "ping -n 3 127.0.0.1 >nul & start chrome http://localhost:8080"

rem Run the server here, so Ctrl+C in this window stops it.
.\chess-web.exe
