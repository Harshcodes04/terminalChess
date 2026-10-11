#!/bin/sh
# ============================================================
#  OWNER : Member D
#  LAYER : Build files
#  FILE  : build.sh
#
#  Only Member D edits this file. Ask D for changes.
# ============================================================

# Builds the game when make is not installed.
g++ -std=c++11 -Wall src/Piece.cpp src/Pieces.cpp src/Board.cpp src/GameState.cpp src/Display.cpp src/InputParser.cpp src/main.cpp -o chess
