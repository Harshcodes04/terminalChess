// This file contains the common code used by all pieces.
// It sets up a piece with its color and position, keeps track of whether
// it has moved, and provides the basic getter functions.
// The actual movement rules for each chess piece are written in Pieces.cpp.

#include "Piece.h"

Piece::Piece(Color c, Position p) {
    color = c;
    pos = p;
    moved = false;
}

Piece::~Piece() {
    // Nothing to free. A piece does not own any memory of its own.
}

Color Piece::getColor() const {
    return color;
}

Position Piece::getPosition() const {
    return pos;
}

void Piece::setPosition(Position p) {
    pos = p;
    moved = true;
}

bool Piece::hasMoved() const {
    return moved;
}
