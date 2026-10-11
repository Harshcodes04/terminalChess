@echo off
rem ============================================================
rem  OWNER : Member D
rem  LAYER : Build files
rem  FILE  : build.bat
rem
rem  Only Member D edits this file. Ask D for changes.
rem ============================================================

rem Builds the game on Windows when make is not installed.
g++ -std=c++11 -Wall src\Piece.cpp src\Pieces.cpp src\Board.cpp src\GameState.cpp src\Display.cpp src\InputParser.cpp src\main.cpp -o chess.exe
if %errorlevel% neq 0 (
    echo Build failed.
) else (
    echo Build finished. Run chess.exe to play.
)
