#ifndef MOVE_H
#define MOVE_H

#include "Position.h"

// One move: the square we pick a piece up from and the square we put it down on.
struct Move {
    Position from;
    Position to;

    Move() {
    }

    Move(Position f, Position t) {
        from = f;
        to = t;
    }
};

#endif
