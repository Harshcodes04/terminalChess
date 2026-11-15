@echo off
rem ============================================================
rem  Builds the browser version of the game.
rem  The terminal version is built by build.bat as before.
rem ============================================================
g++ -std=c++11 -Wall -O2 -static src\Piece.cpp src\Pieces.cpp src\Board.cpp src\GameState.cpp src\Display.cpp src\InputParser.cpp src\server.cpp -o chess-web.exe -lws2_32
if %errorlevel% neq 0 (
    echo Build failed.
) else (
    echo Build finished. Run chess-web.exe, then open http://localhost:8080
)
