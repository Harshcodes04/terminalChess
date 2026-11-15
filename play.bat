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

rem Open Chrome a couple of seconds from now, once the server is listening.
rem The full path to timeout is used because some setups have a different
rem program of the same name earlier in the path.
start "" /b cmd /c "%SystemRoot%\System32\timeout.exe /t 2 /nobreak >nul & start chrome http://localhost:8080"

rem Run the server here, so Ctrl+C in this window stops it.
.\chess-web.exe
