# ============================================================
#  OWNER : Member D
#  LAYER : Build files
#  FILE  : Makefile
#
#  Only Member D edits this file. Ask D for changes.
# ============================================================

CXX = g++
CXXFLAGS = -std=c++11 -Wall

SOURCES = src/Piece.cpp src/Pieces.cpp src/Board.cpp src/GameState.cpp \
          src/Display.cpp src/InputParser.cpp src/main.cpp

chess:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o chess

clean:
	rm -f chess chess.exe
