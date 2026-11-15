// ============================================================
//  OWNER : Member C  (added with the browser version)
//  LAYER : sits on top of the rules layer
//  FILE  : src/Bot.cpp
// ============================================================

#include <algorithm>
#include "Bot.h"

// A score big enough that nothing from counting pieces can reach it, so it
// can only mean checkmate.
static const int MATE = 100000;

// What each piece is worth, in hundredths of a pawn. These are the values
// most chess programs use. The king is not given a value because it is
// never actually captured.
static int pieceValue(char symbol) {
    switch (symbol) {
        case 'P': case 'p': return 100;
        case 'N': case 'n': return 320;
        case 'B': case 'b': return 330;
        case 'R': case 'r': return 500;
        case 'Q': case 'q': return 900;
        default:            return 0;
    }
}

// Knights and bishops are worth more near the middle of the board, because
// they attack more squares from there. Written from White's point of view,
// with rank 1 at the bottom.
static const int CENTRE[8][8] = {
    { -30, -20, -10, -10, -10, -10, -20, -30 },
    { -20, -10,   0,   5,   5,   0, -10, -20 },
    { -10,   0,  10,  15,  15,  10,   0, -10 },
    { -10,   5,  15,  20,  20,  15,   5, -10 },
    { -10,   5,  15,  20,  20,  15,   5, -10 },
    { -10,   0,  10,  15,  15,  10,   0, -10 },
    { -20, -10,   0,   5,   5,   0, -10, -20 },
    { -30, -20, -10, -10, -10, -10, -20, -30 }
};

// A pawn is worth more the closer it gets to the far side, because it is
// closer to becoming a queen. Again written for White.
static const int PAWN_RANK[8] = { 0, 0, 5, 10, 20, 35, 60, 0 };

int Bot::evaluate(const Board& board) {
    int score = 0;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            const Piece* p = board.getPieceAt(Position(r, c));
            if (p == nullptr) {
                continue;
            }

            char symbol = p->getSymbol();
            int value = pieceValue(symbol);

            // Black sees the board upside down, so flip the row when
            // looking its bonuses up.
            int row = (p->getColor() == WHITE) ? r : (7 - r);

            if (symbol == 'N' || symbol == 'n' || symbol == 'B' || symbol == 'b') {
                value += CENTRE[row][c];
            }
            if (symbol == 'P' || symbol == 'p') {
                value += PAWN_RANK[row];
            }

            // White's pieces count upwards, Black's count downwards.
            if (p->getColor() == WHITE) {
                score += value;
            } else {
                score -= value;
            }
        }
    }

    return score;
}

std::vector<Move> allLegalMoves(const GameState& game) {
    std::vector<Move> moves;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Position from(r, c);

            // getLegalMoves already refuses squares that are empty or hold
            // an enemy piece, so this only ever collects our own moves.
            std::vector<Position> targets = game.getLegalMoves(from);
            for (unsigned int i = 0; i < targets.size(); i++) {
                moves.push_back(Move(from, targets[i]));
            }
        }
    }

    return moves;
}

Bot::Bot(int searchDepth) {
    depth = searchDepth;
    if (depth < 1) {
        depth = 1;
    }
}

int Bot::search(const GameState& game, int movesLeft, int alpha, int beta) const {
    // Looked far enough. Just count up the position.
    if (movesLeft == 0) {
        return evaluate(game.getBoard());
    }

    std::vector<Move> moves = allLegalMoves(game);

    // No legal moves means the game has ended here.
    if (moves.empty()) {
        GameStatus status = game.evaluateStatus();
        if (status == CHECKMATE) {
            // Losing is bad for whoever has to move. Mates found sooner
            // score higher, so the bot goes for the quickest one.
            int quickness = movesLeft;
            if (game.getTurn() == WHITE) {
                return -(MATE - quickness);
            }
            return MATE - quickness;
        }
        return 0;   // stalemate, a draw
    }

    bool whiteToPlay = (game.getTurn() == WHITE);
    int best = whiteToPlay ? -MATE : MATE;

    for (unsigned int i = 0; i < moves.size(); i++) {
        // Play the move on a copy, so the real game is untouched. This is
        // the same deep copy the rules layer uses to test for check.
        GameState next(game);
        next.makeMove(moves[i]);

        int score = search(next, movesLeft - 1, alpha, beta);

        if (whiteToPlay) {
            if (score > best) {
                best = score;
            }
            if (best > alpha) {
                alpha = best;
            }
        } else {
            if (score < best) {
                best = score;
            }
            if (best < beta) {
                beta = best;
            }
        }

        // Alpha-beta pruning. The other player would never allow this
        // line, so there is no point looking at the rest of it.
        if (beta <= alpha) {
            break;
        }
    }

    return best;
}

// Used to sort the move list. Written as two small functions rather than
// anything clever so it is easy to read.
static bool betterForWhite(const ScoredMove& a, const ScoredMove& b) {
    return a.score > b.score;
}

static bool betterForBlack(const ScoredMove& a, const ScoredMove& b) {
    return a.score < b.score;
}

std::vector<ScoredMove> Bot::rankMoves(const GameState& game) const {
    std::vector<ScoredMove> ranked;
    std::vector<Move> moves = allLegalMoves(game);

    for (unsigned int i = 0; i < moves.size(); i++) {
        GameState next(game);
        next.makeMove(moves[i]);

        ScoredMove entry;
        entry.move = moves[i];
        entry.score = search(next, depth - 1, -MATE, MATE);
        ranked.push_back(entry);
    }

    if (game.getTurn() == WHITE) {
        std::sort(ranked.begin(), ranked.end(), betterForWhite);
    } else {
        std::sort(ranked.begin(), ranked.end(), betterForBlack);
    }

    return ranked;
}
