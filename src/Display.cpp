

#include <iostream>
#include "Display.h"

static std::string colorName(Color c) {
    if (c == WHITE) {
        return "White";
    }
    return "Black";
}

void Display::render(const Board& board) {
    std::cout << "\n";
    std::cout << "    a   b   c   d   e   f   g   h\n";
    std::cout << "  +---+---+---+---+---+---+---+---+\n";

    // Rank 8 is row 7, and we want it at the top, so we count down.
    for (int r = 7; r >= 0; r--) {
        std::cout << (r + 1) << " |";
        for (int c = 0; c < 8; c++) {
            const Piece* p = board.getPieceAt(Position(r, c));
            if (p == nullptr) {
                std::cout << "   |";
            } else {
                std::cout << " " << p->getSymbol() << " |";
            }
        }
        std::cout << " " << (r + 1) << "\n";
        std::cout << "  +---+---+---+---+---+---+---+---+\n";
    }

    std::cout << "    a   b   c   d   e   f   g   h\n";
    std::cout << "\n";
    std::cout << "  Uppercase = White, lowercase = Black.\n";

    // TODO (remaining work): draw the pieces with the proper Unicode chess
    // glyphs instead of letters. Letters are used for now because not every
    // Windows terminal shows the glyphs correctly.
}

void Display::showStatus(const GameState& game) {
    std::cout << "  Move " << game.getMoveNumber();
    std::cout << "  |  " << colorName(game.getTurn()) << " to play\n";

    // TODO (remaining work): list the captured pieces here. That needs the
    // game to keep hold of captured pieces instead of deleting them, which is
    // a change to Member A's movePiece() and has to be agreed first.
}

void Display::showHistory() {
    // TODO (remaining work): print the list of moves played.
    // Nothing records the moves yet, so there is nothing to print.
    std::cout << "  Move history is not recorded yet.\n";
}

std::string Display::moveResultMessage(MoveResult r) {
    if (r == MOVE_OK) {
        return "";
    }
    if (r == OFF_BOARD) {
        return "That square is not on the board.";
    }
    if (r == NO_PIECE_THERE) {
        return "There is no piece on that square.";
    }
    if (r == NOT_YOUR_PIECE) {
        return "That piece belongs to your opponent.";
    }
    if (r == FRIENDLY_PIECE_THERE) {
        return "One of your own pieces is already on that square.";
    }
    if (r == ILLEGAL_SHAPE) {
        return "That piece cannot move like that.";
    }
    if (r == LEAVES_KING_IN_CHECK) {
        return "You cannot play that, it would leave your king in check.";
    }
    if (r == GAME_ALREADY_OVER) {
        return "The game is already over.";
    }
    return "Move refused.";
}

std::string Display::statusMessage(GameStatus s, Color sideToMove) {
    if (s == CHECK) {
        return colorName(sideToMove) + " is in check.";
    }
    if (s == CHECKMATE) {
        return "Checkmate. " + colorName(otherColor(sideToMove)) + " wins.";
    }
    if (s == STALEMATE) {
        return "Stalemate. The game is a draw.";
    }
    return "";
}

void Display::showHelp() {
    std::cout << "\n";
    std::cout << "  Type a move as the square you are moving from followed by\n";
    std::cout << "  the square you are moving to, for example:  e2e4\n";
    std::cout << "\n";
    std::cout << "  Other commands:\n";
    std::cout << "    help     show this message\n";
    std::cout << "    board    draw the board again\n";
    std::cout << "    history  show the moves played so far\n";
    std::cout << "    quit     leave the game\n";
    std::cout << "\n";
}
