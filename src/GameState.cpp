// ============================================================
//  OWNER : Member C
//  LAYER : Rules and game state
//  FILE  : src/GameState.cpp
//
//  Only Member C edits this file. Ask C for changes.
// ============================================================

#include "GameState.h"

// Helper. Asks whether this colour's king is attacked on the board handed in.
// It is written as a plain function and not a member because we also need to
// ask the question about a throwaway copy of the board, not just our own.
static bool kingIsAttacked(const Board& b, Color c) {
    Position kingSquare = b.findKing(c);
    if (kingSquare.row == -1) {
        // No king on the board, so nothing to attack.
        return false;
    }

    // Walk all 64 squares. For every enemy piece, ask the piece itself whether
    // it could reach the king's square.
    for (int r = 0; r < 8; r++) {
        for (int col = 0; col < 8; col++) {
            Position from(r, col);
            const Piece* p = b.getPieceAt(from);
            if (p == nullptr) {
                continue;
            }
            if (p->getColor() == c) {
                continue;
            }
            Move attack(from, kingSquare);
            if (p->isValidMove(attack, b)) {
                return true;
            }
        }
    }
    return false;
}

GameState::GameState() {
    board.setupInitialPosition();
    turn = WHITE;
    moveNumber = 1;
}

Color GameState::getTurn() const {
    return turn;
}

int GameState::getMoveNumber() const {
    return moveNumber;
}

const Board& GameState::getBoard() const {
    return board;
}

bool GameState::isKingInCheck(Color c) const {
    return kingIsAttacked(board, c);
}

