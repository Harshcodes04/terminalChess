

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Board.h"
#include "Move.h"
#include "Position.h"

// Why a move was accepted or rejected. Separate reasons so the interface can
// print a message that actually says what went wrong.
enum MoveResult {
    MOVE_OK,
    OFF_BOARD,
    NO_PIECE_THERE,
    NOT_YOUR_PIECE,
    FRIENDLY_PIECE_THERE,
    ILLEGAL_SHAPE,
    LEAVES_KING_IN_CHECK,
    GAME_ALREADY_OVER
};

// Where the game stands right now.
enum GameStatus {
    ONGOING,
    CHECK,
    CHECKMATE,
    STALEMATE
};

// Sits between the pieces and the interface.
// Member B's pieces answer "is this move the right shape". This class answers
// "is this move legal in this game right now", and decides when the game is over.
// It also owns whose turn it is, and nothing outside can change that.
class GameState {
private:
    Board board;
    Color turn;
    int moveNumber;

    // Turn order, ownership and friendly fire, then it hands the shape
    // question over to the piece itself.
    MoveResult isLegalMove(Move m) const;

    // Is this colour's king currently attacked?
    bool isKingInCheck(Color c) const;

    // Copy the board, play the move on the copy, and see whether the mover's
    // own king ends up attacked. The real board is never touched.
    bool wouldExposeOwnKing(Move m) const;

    // Does this colour have even one legal move? Stops at the first one it finds.
    bool hasAnyLegalMove(Color c) const;

public:
    GameState();

    // The one way in. Checks the move, plays it, and swaps the turn over,
    // or tells you exactly why it refused.
    MoveResult makeMove(Move m);

    // ONGOING, CHECK, CHECKMATE or STALEMATE for the side that has to move now.
    GameStatus evaluateStatus() const;

    // Read only. Nobody outside gets to change the turn.
    Color getTurn() const;

    int getMoveNumber() const;

    // The interface needs to see the board to draw it. Const, so it can look
    // but not touch.
    const Board& getBoard() const;
};

#endif
