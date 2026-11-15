#!/bin/sh
# Builds the browser version of the game.
g++ -std=c++11 -Wall -O2 src/Piece.cpp src/Pieces.cpp src/Board.cpp src/GameState.cpp src/Display.cpp src/InputParser.cpp src/server.cpp -o chess-web
