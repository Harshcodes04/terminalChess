// ============================================================
//  OWNER : Member B
//  LAYER : Pieces layer
//  FILE  : src/Piece.h
//
//  Only Member B edits this file. Ask B for changes.
// ============================================================

#ifndef PIECE_H
#define PIECE_H

#include <string>
#include "Position.h"
#include "Move.h"

// We only need to know that Board exists here. Board.h includes Piece.h,
// so if Piece.h included Board.h back we would have a circular include.
class Board;

// Abstract base class for every piece.
// This layer answers geometry questions only. It does not know about
// turns, check, or whose move it is. That is Member C's job.
class Piece {
protected:
    Color color;
    Position pos;
    bool moved;

public:
    Piece(Color c, Position p);

    // Virtual destructor. Pieces are deleted through a Piece* by the board,
    // so without this the subclass destructor would never run.
    virtual ~Piece();

    // ---- the four things every piece must answer for itself ----

    // Can this piece geometrically make this move on this board?
    virtual bool isValidMove(Move m, const Board& board) const = 0;

    // The letter used to draw this piece. Uppercase for white, lowercase for black.
    virtual char getSymbol() const = 0;

    // Readable name, used in messages.
    virtual std::string getName() const = 0;

    // Make an independent copy of this piece. The board needs this to deep copy
    // itself without knowing what kind of piece it is holding.
    virtual Piece* clone() const = 0;

    // ---- written once here, the same for all pieces ----

    Color getColor() const;
    Position getPosition() const;
    void setPosition(Position p);
    bool hasMoved() const;
};

#endif
