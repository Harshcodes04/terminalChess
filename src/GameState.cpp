
#include "GameState.h"

// Asks whether this colour's king is attacked on the board handed in.
//  plain function and not a member because we also need to
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

MoveResult GameState::isLegalMove(Move m) const {
    // Both squares have to be real squares.
    if (!isOnBoard(m.from) || !isOnBoard(m.to)) {
        return OFF_BOARD;
    }

    // Moving a piece onto the square it already stands on is not a move.
    if (samePosition(m.from, m.to)) {
        return ILLEGAL_SHAPE;
    }

    // 1. Is there a piece to move at all?
    const Piece* mover = board.getPieceAt(m.from);
    if (mover == nullptr) {
        return NO_PIECE_THERE;
    }

    // 2. Is it ours?
    if (mover->getColor() != turn) {
        return NOT_YOUR_PIECE;
    }

    // 3. Are we trying to land on one of our own pieces?
    const Piece* target = board.getPieceAt(m.to);
    if (target != nullptr && target->getColor() == mover->getColor()) {
        return FRIENDLY_PIECE_THERE;
    }

    // 4. Is the move the right shape for this piece? Only the piece knows.
    if (!mover->isValidMove(m, board)) {
        return ILLEGAL_SHAPE;
    }

    return MOVE_OK;
}

bool GameState::wouldExposeOwnKing(Move m) const {
    const Piece* mover = board.getPieceAt(m.from);
    if (mover == nullptr) {
        return false;
    }
    Color mine = mover->getColor();

    // Deep copy, so whatever we do next cannot touch the real game.
    Board trial(board);
    trial.movePiece(m);

    return kingIsAttacked(trial, mine);
}

bool GameState::hasAnyLegalMove(Color c) const {
    // Try every piece of this colour against every square on the board and
    // stop the moment one move works.
    for (int r = 0; r < 8; r++) {
        for (int col = 0; col < 8; col++) {
            Position from(r, col);
            const Piece* p = board.getPieceAt(from);
            if (p == nullptr || p->getColor() != c) {
                continue;
            }

            for (int r2 = 0; r2 < 8; r2++) {
                for (int c2 = 0; c2 < 8; c2++) {
                    Position to(r2, c2);
                    if (samePosition(from, to)) {
                        continue;
                    }

                    // Same tests as isLegalMove, but written out here so this
                    // works for either colour and not just the side to move.
                    const Piece* target = board.getPieceAt(to);
                    if (target != nullptr && target->getColor() == c) {
                        continue;
                    }

                    Move m(from, to);
                    if (!p->isValidMove(m, board)) {
                        continue;
                    }
                    if (wouldExposeOwnKing(m)) {
                        continue;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

MoveResult GameState::makeMove(Move m) {
    // Refuse to carry on once the game is finished.
    GameStatus before = evaluateStatus();
    if (before == CHECKMATE || before == STALEMATE) {
        return GAME_ALREADY_OVER;
    }

    // The cheap tests first.
    MoveResult result = isLegalMove(m);
    if (result != MOVE_OK) {
        return result;
    }

    // The expensive test last, because it copies the whole board, and by now
    // the move has already survived everything else.
    if (wouldExposeOwnKing(m)) {
        return LEAVES_KING_IN_CHECK;
    }

    board.movePiece(m);

    // Black moving ends the full move, which is when the number goes up.
    if (turn == BLACK) {
        moveNumber++;
    }
    turn = otherColor(turn);

    return MOVE_OK;
}

GameStatus GameState::evaluateStatus() const {
    bool inCheck = isKingInCheck(turn);

    if (!hasAnyLegalMove(turn)) {
        if (inCheck) {
            return CHECKMATE;
        }
        return STALEMATE;
    }

    if (inCheck) {
        return CHECK;
    }
    return ONGOING;
}
