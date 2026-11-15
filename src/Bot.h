// ============================================================
//  OWNER : Member C  (added with the browser version)
//  LAYER : sits on top of the rules layer
//  FILE  : src/Bot.h
// ============================================================
//
// A simple computer player that can also act as a coach.
//
// It does not know any chess rules of its own. It asks GameState for the
// legal moves, plays them on a copy of the game, and looks a few moves
// ahead to see which one turns out best. Everything it knows about chess
// comes from the engine underneath it.

#ifndef BOT_H
#define BOT_H

#include <vector>
#include "GameState.h"

// One move together with how good the bot thinks it is.
// The score is in hundredths of a pawn, counted from White's side, so a
// score of +100 means White is a pawn ahead and -100 means Black is.
struct ScoredMove {
    Move move;
    int score;
};

class Bot {
public:
    // depth is how many half moves it looks ahead. 1 is almost blind,
    // 3 plays a reasonable game, 4 is slower but stronger.
    Bot(int searchDepth);

    // Scores every legal move in this position, best first for the side
    // whose turn it is. This is the one call the coach and the opponent
    // both use: the opponent plays the first move in the list, and the
    // coach compares the player's move against it.
    std::vector<ScoredMove> rankMoves(const GameState& game) const;

    // How good this position is, counted from White's side.
    static int evaluate(const Board& board);

private:
    int depth;

    // Minimax with alpha-beta pruning. White tries to make the score as
    // large as possible and Black tries to make it as small as possible.
    int search(const GameState& game, int movesLeft, int alpha, int beta) const;
};

// Every legal move for the player whose turn it is.
std::vector<Move> allLegalMoves(const GameState& game);

#endif
