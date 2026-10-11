// ============================================================
//  OWNER : Member A
//  LAYER : Board layer
//  FILE  : src/Board.h
//
//  Only Member A edits this file. Ask A for changes.
// ============================================================

#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include "Move.h"
#include "Position.h"

//
// The board owns the 8x8 grid and every piece standing on it. The grid is
// private and nobody outside gets a pointer they could change a piece through,
// so the only way the board can change is movePiece().
class Board {
private:
    Piece* grid[8][8];

    // Deletes every piece and sets all 64 squares back to empty.
    void clearBoard();

    // We never assign one board to another, we only copy construct.
    // Declaring this and not writing it means the compiler stops anyone who
    // tries, instead of silently making a shallow copy.
    Board& operator=(const Board& other);

public:
    // Empty board, all 64 squares null.
    Board();

    // Deletes every piece the board is holding.
    ~Board();

    // Deep copy. Every piece is built fresh with clone(), so the copy and the
    // original never share a piece. Member C needs this to try a move out on a
    // throwaway board.
    Board(const Board& other);

    // Puts all 32 pieces on their starting squares.
    void setupInitialPosition();

    // The piece standing on a square, or nullptr if the square is empty.
    // Const pointer, so callers can ask questions but cannot change the piece.
    const Piece* getPieceAt(Position p) const;

    // Is this square free?
    bool isEmpty(Position p) const;

    // Is every square strictly between these two squares empty?
    // The two end squares themselves are not looked at. Used by the rook,
    // the bishop and the queen.
    bool isPathClear(Position from, Position to) const;

    // The only function that changes the board. Moves the piece and deletes
    // whatever it captured.
    void movePiece(Move m);

    // Where is this colour's king? Returns row -1 if there is no king,
    // which should never happen in a real game.
    Position findKing(Color c) const;
};

#endif
