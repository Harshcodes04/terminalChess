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

rem Open Chrome a moment from now, once the server is actually listening.
start "" /b cmd /c "%SystemRoot%\System32	imeout.exe /t 2 /nobreak >nul & start chrome http://localhost:8080"

rem Run the server here, so Ctrl+C in this window stops it.
.\chess-web.exe
