// ============================================================
//  OWNER : Member B
//  LAYER : Pieces layer
//  FILE  : src/Pieces.h
//
//  Only Member B edits this file. Ask B for changes.
// ============================================================

#ifndef PIECES_H
#define PIECES_H

#include "Piece.h"

// The six concrete pieces. Each one overrides isValidMove() and clone().

class Pawn : public Piece {
public:
    Pawn(Color c, Position p);
    bool isValidMove(Move m, const Board& board) const;
    char getSymbol() const;
    std::string getName() const;
    Piece* clone() const;
};

class Rook : public Piece {
public:
    Rook(Color c, Position p);
    bool isValidMove(Move m, const Board& board) const;
    char getSymbol() const;
    std::string getName() const;
    Piece* clone() const;
};

class Knight : public Piece {
public:
    Knight(Color c, Position p);
    bool isValidMove(Move m, const Board& board) const;
    char getSymbol() const;
    std::string getName() const;
    Piece* clone() const;
};

class Bishop : public Piece {
public:
    Bishop(Color c, Position p);
    bool isValidMove(Move m, const Board& board) const;
    char getSymbol() const;
    std::string getName() const;
    Piece* clone() const;
};

class Queen : public Piece {
public:
    Queen(Color c, Position p);
    bool isValidMove(Move m, const Board& board) const;
    char getSymbol() const;
    std::string getName() const;
    Piece* clone() const;
};

class King : public Piece {
public:
    King(Color c, Position p);
    bool isValidMove(Move m, const Board& board) const;
    char getSymbol() const;
    std::string getName() const;
    Piece* clone() const;
};

#endif
