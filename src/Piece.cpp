// ============================================================
//  OWNER : Member B
//  LAYER : Pieces layer
//  FILE  : src/Piece.cpp
//
//  Only Member B edits this file. Ask B for changes.
// ============================================================

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
