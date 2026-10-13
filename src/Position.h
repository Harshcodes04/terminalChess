#ifndef POSITION_H
#define POSITION_H

// Which side a piece belongs to.
enum Color {
    WHITE,
    BLACK
};

// A square on the board.
// row 0 is rank 1 (white's home row), row 7 is rank 8.
// col 0 is file a, col 7 is file h.
struct Position {
    int row;
    int col;

    Position() {
        row = 0;
        col = 0;
    }

    Position(int r, int c) {
        row = r;
        col = c;
    }
};

// True if the square is actually on the 8x8 board.
inline bool isOnBoard(Position p) {
    return p.row >= 0 && p.row < 8 && p.col >= 0 && p.col < 8;
}

// True if the two squares are the same square.
inline bool samePosition(Position a, Position b) {
    return a.row == b.row && a.col == b.col;
}

// The other colour.
inline Color otherColor(Color c) {
    if (c == WHITE) {
        return BLACK;
    }
    return WHITE;
}

#endif
