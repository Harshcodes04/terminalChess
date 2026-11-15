
#include "Board.h"
#include "Pieces.h"

Board::Board() {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            grid[r][c] = nullptr;
        }
    }
    enPassantSquare = Position(-1, -1);
}

Board::~Board() {
    clearBoard();
}

Board::Board(const Board& other) {
    // The en passant chance is part of the position, so a copy of the board
    // has to carry it as well, otherwise a simulated move would lose it.
    enPassantSquare = other.enPassantSquare;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (other.grid[r][c] == nullptr) {
                grid[r][c] = nullptr;
            } else {
                // clone() gives us a brand new piece of the right kind without
                // us having to ask what kind it is.
                grid[r][c] = other.grid[r][c]->clone();
            }
        }
    }
}

void Board::clearBoard() {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (grid[r][c] != nullptr) {
                delete grid[r][c];
                grid[r][c] = nullptr;
            }
        }
    }
}

void Board::setupInitialPosition() {
    // Start from a clean board in case this is called twice.
    clearBoard();
    enPassantSquare = Position(-1, -1);

    // Pawns.
    for (int c = 0; c < 8; c++) {
        grid[1][c] = new Pawn(WHITE, Position(1, c));
        grid[6][c] = new Pawn(BLACK, Position(6, c));
    }

    // White back row, rank 1.
    grid[0][0] = new Rook(WHITE, Position(0, 0));
    grid[0][1] = new Knight(WHITE, Position(0, 1));
    grid[0][2] = new Bishop(WHITE, Position(0, 2));
    grid[0][3] = new Queen(WHITE, Position(0, 3));
    grid[0][4] = new King(WHITE, Position(0, 4));
    grid[0][5] = new Bishop(WHITE, Position(0, 5));
    grid[0][6] = new Knight(WHITE, Position(0, 6));
    grid[0][7] = new Rook(WHITE, Position(0, 7));

    // Black back row, rank 8.
    grid[7][0] = new Rook(BLACK, Position(7, 0));
    grid[7][1] = new Knight(BLACK, Position(7, 1));
    grid[7][2] = new Bishop(BLACK, Position(7, 2));
    grid[7][3] = new Queen(BLACK, Position(7, 3));
    grid[7][4] = new King(BLACK, Position(7, 4));
    grid[7][5] = new Bishop(BLACK, Position(7, 5));
    grid[7][6] = new Knight(BLACK, Position(7, 6));
    grid[7][7] = new Rook(BLACK, Position(7, 7));
}

const Piece* Board::getPieceAt(Position p) const {
    if (!isOnBoard(p)) {
        return nullptr;
    }
    return grid[p.row][p.col];
}

bool Board::isEmpty(Position p) const {
    return getPieceAt(p) == nullptr;
}

bool Board::isPathClear(Position from, Position to) const {
    int dr = to.row - from.row;
    int dc = to.col - from.col;

    // This only makes sense along a rank, a file or a diagonal.
    bool straight = (dr == 0 || dc == 0);
    bool diagonal = (dr == dc || dr == -dc);
    if (!straight && !diagonal) {
        return false;
    }

    // Work out which way to walk: -1, 0 or +1 on each axis.
    int stepRow = 0;
    if (dr > 0) {
        stepRow = 1;
    } else if (dr < 0) {
        stepRow = -1;
    }

    int stepCol = 0;
    if (dc > 0) {
        stepCol = 1;
    } else if (dc < 0) {
        stepCol = -1;
    }

    // Start one square along and stop one square short, so the two end
    // squares are never checked.
    int r = from.row + stepRow;
    int c = from.col + stepCol;
    while (r != to.row || c != to.col) {
        if (grid[r][c] != nullptr) {
            return false;
        }
        r = r + stepRow;
        c = c + stepCol;
    }
    return true;
}

void Board::movePiece(Move m) {
    if (!isOnBoard(m.from) || !isOnBoard(m.to)) {
        return;
    }

    Piece* mover = grid[m.from.row][m.from.col];
    if (mover == nullptr) {
        return;
    }

    char symbol = mover->getSymbol();
    bool isPawn = (symbol == 'P' || symbol == 'p');
    bool isKing = (symbol == 'K' || symbol == 'k');

    // Remember the old en passant square before we change it, because the
    // move being played now may be the capture that uses it.
    Position oldEnPassant = enPassantSquare;
    enPassantSquare = Position(-1, -1);

    // --- en passant capture ---
    // A pawn moving diagonally onto an empty square can only be an en
    // passant capture. The pawn it takes is beside the start square, not
    // on the square the capturing pawn lands on.
    if (isPawn && m.from.col != m.to.col && grid[m.to.row][m.to.col] == nullptr) {
        if (oldEnPassant.row == m.to.row && oldEnPassant.col == m.to.col) {
            Piece* victim = grid[m.from.row][m.to.col];
            if (victim != nullptr) {
                delete victim;
                grid[m.from.row][m.to.col] = nullptr;
            }
        }
    }

    // Anything already standing on the destination is captured, and since the
    // board owns its pieces the board is the one that deletes it.
    if (grid[m.to.row][m.to.col] != nullptr) {
        delete grid[m.to.row][m.to.col];
        grid[m.to.row][m.to.col] = nullptr;
    }

    grid[m.to.row][m.to.col] = mover;
    grid[m.from.row][m.from.col] = nullptr;
    mover->setPosition(m.to);

    // --- castling ---
    // The king has already moved two squares sideways, so the rook on that
    // side has to jump over it and land beside it.
    if (isKing && (m.to.col - m.from.col == 2 || m.from.col - m.to.col == 2)) {
        int row = m.from.row;
        int rookFrom = (m.to.col > m.from.col) ? 7 : 0;
        int rookTo   = (m.to.col > m.from.col) ? 5 : 3;

        Piece* rook = grid[row][rookFrom];
        if (rook != nullptr) {
            grid[row][rookTo] = rook;
            grid[row][rookFrom] = nullptr;
            rook->setPosition(Position(row, rookTo));
        }
    }

    // --- a pawn jumping two squares ---
    // Note the square it skipped, so that an enemy pawn gets its one
    // chance to capture into it on the very next move.
    if (isPawn && (m.to.row - m.from.row == 2 || m.from.row - m.to.row == 2)) {
        int middle = (m.from.row + m.to.row) / 2;
        enPassantSquare = Position(middle, m.from.col);
    }

    // --- promotion ---
    // A pawn that reaches the far row turns into something better. The
    // old pawn is deleted and a brand new piece takes its place.
    if (isPawn && (m.to.row == 0 || m.to.row == 7)) {
        Color colour = mover->getColor();
        char wanted = m.promotion;
        if (wanted == 0) {
            wanted = 'Q';      // nothing chosen, so give them a queen
        }

        delete grid[m.to.row][m.to.col];
        grid[m.to.row][m.to.col] = nullptr;

        Piece* promoted = nullptr;
        if (wanted == 'R' || wanted == 'r') {
            promoted = new Rook(colour, m.to);
        } else if (wanted == 'B' || wanted == 'b') {
            promoted = new Bishop(colour, m.to);
        } else if (wanted == 'N' || wanted == 'n') {
            promoted = new Knight(colour, m.to);
        } else {
            promoted = new Queen(colour, m.to);
        }

        grid[m.to.row][m.to.col] = promoted;
    }
}

Position Board::getEnPassantSquare() const {
    return enPassantSquare;
}

Position Board::findKing(Color c) const {
    char wanted = 'K';
    if (c == BLACK) {
        wanted = 'k';
    }

    for (int r = 0; r < 8; r++) {
        for (int col = 0; col < 8; col++) {
            if (grid[r][col] != nullptr && grid[r][col]->getSymbol() == wanted) {
                return Position(r, col);
            }
        }
    }

    // No king found. Row -1 is our way of saying "nowhere".
    return Position(-1, -1);
}
