#ifndef MOVE_H
#define MOVE_H

#include "Position.h"

// One move: the square we pick a piece up from and the square we put it down on.
struct Move {
    Position from;
    Position to;

    // Only used when a pawn reaches the far side. Holds the letter of the
    // piece the player wants: 'Q', 'R', 'B' or 'N'. Left as 0 for every
    // other move, and a 0 here is treated as a queen.
    char promotion;

    Move() {
        promotion = 0;
    }

    Move(Position f, Position t) {
        from = f;
        to = t;
        promotion = 0;
    }

    Move(Position f, Position t, char promoteTo) {
        from = f;
        to = t;
        promotion = promoteTo;
    }
};

#endif
