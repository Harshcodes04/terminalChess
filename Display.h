// ============================================================
//  OWNER : Member D
//  LAYER : Interface layer
//  FILE  : src/Display.h
//
//  Only Member D edits this file. Ask D for changes.
// ============================================================

#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include "Board.h"
#include "GameState.h"

//
// Everything the player sees comes from here. None of the other modules
// print anything, so the logic can be tested without any screen output.
class Display {
public:
    // Draws the board with rank numbers down the side and file letters
    // underneath. Rank 8 is printed at the top, the way a chess diagram looks.
    static void render(const Board& board);

    // Move number and whose turn it is.
    static void showStatus(const GameState& game);

    // Prints the list of moves played so far.
    static void showHistory();

    // Turns a rejection reason into something a human can read.
    static std::string moveResultMessage(MoveResult r);

    // Turns the game status into a line to print, or "" when there is
    // nothing worth saying.
    static std::string statusMessage(GameStatus s, Color sideToMove);

    static void showHelp();
};

#endif
