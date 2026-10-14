
#include "Board.h"
#include "Pieces.h"

Board::Board() {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            grid[r][c] = nullptr;
        }
    }
}

Board::~Board() {
    clearBoard();
}

Board::Board(const Board& other) {
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

    // Anything already standing on the destination is captured, and since the
    // board owns its pieces the board is the one that deletes it.
    if (grid[m.to.row][m.to.col] != nullptr) {
        delete grid[m.to.row][m.to.col];
        grid[m.to.row][m.to.col] = nullptr;
    }

    grid[m.to.row][m.to.col] = mover;
    grid[m.from.row][m.from.col] = nullptr;
    mover->setPosition(m.to);
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
