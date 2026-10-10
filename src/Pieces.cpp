// This file has the actual rules for how each piece moves.
// For example, pawns move forward, rooks move in straight lines,
// bishops move diagonally, and knights move in an L shape.
// It also gives each piece its board symbol, name, and clone function.
// Bigger game rules like turns, check, and checkmate are handled elsewhere.

#include "Pieces.h"
#include "Board.h"

// Small helper so we do not have to include <cstdlib> just for abs().
static int absValue(int x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

// Shared by the rook, the bishop and the queen, so the same path walk is not
// written out three times (see section 3.3 of the design document).
// allowStraight  - may the piece move along a rank or a file
// allowDiagonal  - may the piece move along a diagonal
static bool slidingMoveIsValid(Move m, const Board& board,
                               bool allowStraight, bool allowDiagonal) {
    int dr = m.to.row - m.from.row;
    int dc = m.to.col - m.from.col;

    // Staying still is not a move.
    if (dr == 0 && dc == 0) {
        return false;
    }

    bool straight = (dr == 0 || dc == 0);
    bool diagonal = (dr == dc || dr == -dc);

    if (straight) {
        if (!allowStraight) {
            return false;
        }
    } else if (diagonal) {
        if (!allowDiagonal) {
            return false;
        }
    } else {
        // Not a straight line and not a diagonal, so no sliding piece can do it.
        return false;
    }

    // The squares in between must be empty. The destination itself is allowed
    // to hold a piece, because that is how a capture looks.
    return board.isPathClear(m.from, m.to);
}

// ------------------------------- Pawn -------------------------------
// Done first because it is the most awkward piece: its direction depends on
// its colour, it captures diagonally but advances straight, and it gets a
// two square move on its first turn.

Pawn::Pawn(Color c, Position p) : Piece(c, p) {
}

bool Pawn::isValidMove(Move m, const Board& board) const {
    // White walks up the board (row increases), black walks down.
    int dir = 1;
    if (color == BLACK) {
        dir = -1;
    }

    int dr = m.to.row - m.from.row;
    int dc = m.to.col - m.from.col;
    const Piece* target = board.getPieceAt(m.to);

    // One square straight ahead, only onto an empty square.
    if (dc == 0 && dr == dir && target == nullptr) {
        return true;
    }

    // Two squares straight ahead, only from the pawn's own starting row,
    // and only if both squares are empty.
    int startRow = 1;
    if (color == BLACK) {
        startRow = 6;
    }
    if (dc == 0 && dr == 2 * dir && m.from.row == startRow) {
        Position middle(m.from.row + dir, m.from.col);
        if (board.isEmpty(middle) && target == nullptr) {
            return true;
        }
    }

    // One square diagonally, but only to take an enemy piece.
    if ((dc == 1 || dc == -1) && dr == dir) {
        if (target != nullptr && target->getColor() != color) {
            return true;
        }
    }

    // TODO (remaining work): en passant, and promotion when the pawn reaches
    // the last row. Neither is handled yet.
    return false;
}

char Pawn::getSymbol() const {
    if (color == WHITE) {
        return 'P';
    }
    return 'p';
}

std::string Pawn::getName() const {
    return "Pawn";
}

Piece* Pawn::clone() const {
    Pawn* copy = new Pawn(color, pos);
    copy->moved = moved;
    return copy;
}

// ------------------------------- Rook -------------------------------

Rook::Rook(Color c, Position p) : Piece(c, p) {
}

bool Rook::isValidMove(Move m, const Board& board) const {
    // Straight lines only.
    return slidingMoveIsValid(m, board, true, false);
}

char Rook::getSymbol() const {
    if (color == WHITE) {
        return 'R';
    }
    return 'r';
}

std::string Rook::getName() const {
    return "Rook";
}

Piece* Rook::clone() const {
    Rook* copy = new Rook(color, pos);
    copy->moved = moved;
    return copy;
}

// ------------------------------ Knight ------------------------------

Knight::Knight(Color c, Position p) : Piece(c, p) {
}

bool Knight::isValidMove(Move m, const Board& board) const {
    // The board is not looked at here. The knight jumps over whatever is in
    // the way, so there is no path to check. The parameter is still there
    // because the base class decides the signature for every piece.
    int dr = absValue(m.to.row - m.from.row);
    int dc = absValue(m.to.col - m.from.col);

    // The L shape. The knight jumps, so we never look at the squares between.
    if (dr == 1 && dc == 2) {
        return true;
    }
    if (dr == 2 && dc == 1) {
        return true;
    }
    return false;
}

char Knight::getSymbol() const {
    if (color == WHITE) {
        return 'N';
    }
    return 'n';
}

std::string Knight::getName() const {
    return "Knight";
}

Piece* Knight::clone() const {
    Knight* copy = new Knight(color, pos);
    copy->moved = moved;
    return copy;
}

// ------------------------------ Bishop ------------------------------

Bishop::Bishop(Color c, Position p) : Piece(c, p) {
}

bool Bishop::isValidMove(Move m, const Board& board) const {
    // Diagonals only.
    return slidingMoveIsValid(m, board, false, true);
}

char Bishop::getSymbol() const {
    if (color == WHITE) {
        return 'B';
    }
    return 'b';
}

std::string Bishop::getName() const {
    return "Bishop";
}

Piece* Bishop::clone() const {
    Bishop* copy = new Bishop(color, pos);
    copy->moved = moved;
    return copy;
}

// ------------------------------- Queen ------------------------------

Queen::Queen(Color c, Position p) : Piece(c, p) {
}

bool Queen::isValidMove(Move m, const Board& board) const {
    // Straight lines and diagonals, which is why the helper takes two flags.
    return slidingMoveIsValid(m, board, true, true);
}

char Queen::getSymbol() const {
    if (color == WHITE) {
        return 'Q';
    }
    return 'q';
}

std::string Queen::getName() const {
    return "Queen";
}

Piece* Queen::clone() const {
    Queen* copy = new Queen(color, pos);
    copy->moved = moved;
    return copy;
}

// ------------------------------- King -------------------------------
// Done last, because whether a king move is actually allowed also depends on
// check detection, which lives in Member C's module.

King::King(Color c, Position p) : Piece(c, p) {
}

bool King::isValidMove(Move m, const Board& board) const {
    // The board is not looked at here either. The king only moves one square,
    // so there is never anything in between to block it.
    int dr = absValue(m.to.row - m.from.row);
    int dc = absValue(m.to.col - m.from.col);

    // One square in any direction, and it has to actually move.
    if (dr <= 1 && dc <= 1 && !(dr == 0 && dc == 0)) {
        return true;
    }

    // TODO (remaining work): castling.
    return false;
}

char King::getSymbol() const {
    if (color == WHITE) {
        return 'K';
    }
    return 'k';
}

std::string King::getName() const {
    return "King";
}

Piece* King::clone() const {
    King* copy = new King(color, pos);
    copy->moved = moved;
    return copy;
}
