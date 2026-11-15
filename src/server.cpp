// ============================================================
//  OWNER : Member D
//  LAYER : Interface layer
//  FILE  : src/server.cpp
//
//  Only Member D edits this file. Ask D for changes.
// ============================================================
//
// A small web version of the game. It keeps one GameState in memory and
// answers a few simple requests from the browser. The chess engine itself
// is not changed at all: this file only asks it questions, exactly the way
// the terminal version does.

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

#include "../third_party/httplib.h"
#include "GameState.h"
#include "InputParser.h"
#include "Display.h"
#include "Bot.h"

// The one game everybody is looking at.
static GameState game;

// ---------- small helpers for building JSON by hand ----------
// We write the JSON text ourselves instead of using a library, because the
// replies are short and this keeps the project free of extra dependencies.

static std::string quoted(const std::string& s) {
    return "\"" + s + "\"";
}

// Turns a square into text like "e4".
static std::string squareName(Position p) {
    return InputParser::positionToText(p);
}

// Reads a single square such as "e2". Returns false if the text is not a
// square. The move parser only understands whole moves, so this does the
// one square case on its own rather than pretending it is a move.
static bool parseSquare(const std::string& text, Position& out) {
    if (text.size() != 2) {
        return false;
    }

    char file = text[0];
    char rank = text[1];

    if (file >= 'A' && file <= 'H') {
        file = file + ('a' - 'A');
    }
    if (file < 'a' || file > 'h') {
        return false;
    }
    if (rank < '1' || rank > '8') {
        return false;
    }

    out = Position(rank - '1', file - 'a');
    return true;
}

// The whole board as a list of 64 entries, read left to right, rank 8 first,
// which is the order the browser draws them in.
static std::string boardToJson(const Board& b) {
    std::string out = "[";
    bool first = true;

    for (int r = 7; r >= 0; r--) {
        for (int c = 0; c < 8; c++) {
            Position pos(r, c);
            const Piece* p = b.getPieceAt(pos);

            if (!first) {
                out += ",";
            }
            first = false;

            out += "{";
            out += quoted("square") + ":" + quoted(squareName(pos)) + ",";

            if (p == nullptr) {
                out += quoted("piece") + ":null";
            } else {
                std::string symbol(1, p->getSymbol());
                std::string colour = (p->getColor() == WHITE) ? "white" : "black";
                out += quoted("piece") + ":" + quoted(symbol) + ",";
                out += quoted("colour") + ":" + quoted(colour) + ",";
                out += quoted("name") + ":" + quoted(p->getName());
            }
            out += "}";
        }
    }

    out += "]";
    return out;
}

static std::string statusWord(GameStatus s) {
    if (s == CHECK) {
        return "check";
    }
    if (s == CHECKMATE) {
        return "checkmate";
    }
    if (s == STALEMATE) {
        return "stalemate";
    }
    return "ongoing";
}

// Everything the browser needs in order to draw one screen.
static std::string stateToJson() {
    GameStatus status = game.evaluateStatus();
    std::string turn = (game.getTurn() == WHITE) ? "white" : "black";

    std::string out = "{";
    out += quoted("board") + ":" + boardToJson(game.getBoard()) + ",";
    out += quoted("turn") + ":" + quoted(turn) + ",";
    out += quoted("moveNumber") + ":" + std::to_string(game.getMoveNumber()) + ",";
    out += quoted("status") + ":" + quoted(statusWord(status));
    out += "}";
    return out;
}

// Reads a file off disk and returns it as text.
static std::string readFile(const std::string& path) {
    std::ifstream in(path.c_str());
    if (!in) {
        return "";
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

// How far ahead the bot looks. 3 is quick and plays sensibly, 4 is
// stronger and takes about half a second.
static int botDepth = 3;

// Reads a depth off the request, keeping it inside sensible limits so a
// stray value cannot make the browser wait forever.
static int depthFrom(const httplib::Request& req) {
    std::string text = req.get_param_value("depth");
    if (text.empty()) {
        return botDepth;
    }
    int d = atoi(text.c_str());
    if (d < 1) {
        d = 1;
    }
    if (d > 4) {
        d = 4;
    }
    return d;
}

// Turns a move into text like "e2e4".
static std::string moveName(Move m) {
    return squareName(m.from) + squareName(m.to);
}

// How much worse the played move was than the best one available, counted
// in hundredths of a pawn. Always zero or more.
static int lossFor(Color mover, int bestScore, int playedScore) {
    if (mover == WHITE) {
        return bestScore - playedScore;   // White wants the score high
    }
    return playedScore - bestScore;       // Black wants it low
}

// The word shown to the player. The bands are the ones chess sites use.
static std::string verdictFor(int loss) {
    if (loss <= 0) {
        return "Best move";
    }
    if (loss < 30) {
        return "Good";
    }
    if (loss < 100) {
        return "Inaccuracy";
    }
    if (loss < 200) {
        return "Mistake";
    }
    return "Blunder";
}

int main() {
    httplib::Server server;

    // The page itself.
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::string page = readFile("web/index.html");
        if (page.empty()) {
            res.status = 500;
            res.set_content("Could not find web/index.html. Run the server from "
                            "the project folder.", "text/plain");
            return;
        }
        res.set_content(page, "text/html");
    });

    // Where the board stands right now.
    server.Get("/state", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(stateToJson(), "application/json");
    });

    // Every square the piece on ?from=e2 may legally move to.
    // This is what lights the board up when a player picks a piece.
    server.Get("/moves", [](const httplib::Request& req, httplib::Response& res) {
        std::string from = req.get_param_value("from");

        Position square;
        if (!parseSquare(from, square)) {
            res.set_content("[]", "application/json");
            return;
        }

        std::vector<Position> squares = game.getLegalMoves(square);

        std::string out = "[";
        for (unsigned int i = 0; i < squares.size(); i++) {
            if (i > 0) {
                out += ",";
            }
            out += quoted(squareName(squares[i]));
        }
        out += "]";

        res.set_content(out, "application/json");
    });

    // Play a move. The body is just the text, for example "e2e4".
    // Adding ?coach=1 also has the bot judge the move.
    server.Post("/move", [](const httplib::Request& req, httplib::Response& res) {
        Move m;
        if (!InputParser::parse(req.body, m)) {
            res.set_content("{\"ok\":false,\"reason\":\"I did not understand that move.\"}",
                            "application/json");
            return;
        }

        bool coaching = (req.get_param_value("coach") == "1");
        Color mover = game.getTurn();

        // Score every move available before the player commits to one.
        // The same list gives both the best move and the player's move,
        // so the position only has to be searched once.
        std::vector<ScoredMove> ranked;
        if (coaching) {
            Bot bot(depthFrom(req));
            ranked = bot.rankMoves(game);
        }

        MoveResult result = game.makeMove(m);

        std::string out = "{";
        if (result == MOVE_OK) {
            out += quoted("ok") + ":true,";
            out += quoted("reason") + ":" + quoted("") + ",";
        } else {
            out += quoted("ok") + ":false,";
            out += quoted("reason") + ":" + quoted(Display::moveResultMessage(result)) + ",";
        }

        // The verdict, if the player asked for coaching and the move was
        // actually played.
        if (coaching && result == MOVE_OK && !ranked.empty()) {
            int bestScore = ranked[0].score;
            std::string bestMove = moveName(ranked[0].move);

            bool foundPlayed = false;
            int playedScore = bestScore;
            for (unsigned int i = 0; i < ranked.size(); i++) {
                if (samePosition(ranked[i].move.from, m.from) &&
                    samePosition(ranked[i].move.to, m.to)) {
                    playedScore = ranked[i].score;
                    foundPlayed = true;
                    break;
                }
            }

            if (foundPlayed) {
                int loss = lossFor(mover, bestScore, playedScore);
                if (loss < 0) {
                    loss = 0;
                }
                out += quoted("verdict") + ":" + quoted(verdictFor(loss)) + ",";
                out += quoted("loss") + ":" + std::to_string(loss) + ",";
                out += quoted("best") + ":" + quoted(bestMove) + ",";
                out += quoted("score") + ":" + std::to_string(playedScore) + ",";
            }
        }

        out += quoted("state") + ":" + stateToJson();
        out += "}";

        res.set_content(out, "application/json");
    });

    // Let the bot play the side whose turn it is.
    server.Post("/botmove", [](const httplib::Request& req, httplib::Response& res) {
        Bot bot(depthFrom(req));
        std::vector<ScoredMove> ranked = bot.rankMoves(game);

        if (ranked.empty()) {
            res.set_content("{\"ok\":false,\"reason\":\"No moves left.\",\"state\":"
                            + stateToJson() + "}", "application/json");
            return;
        }

        Move chosen = ranked[0].move;
        game.makeMove(chosen);

        std::string out = "{";
        out += quoted("ok") + ":true,";
        out += quoted("move") + ":" + quoted(moveName(chosen)) + ",";
        out += quoted("score") + ":" + std::to_string(ranked[0].score) + ",";
        out += quoted("state") + ":" + stateToJson();
        out += "}";

        res.set_content(out, "application/json");
    });

    // What the bot would play here, without playing it. Used for a hint.
    server.Get("/hint", [](const httplib::Request& req, httplib::Response& res) {
        Bot bot(depthFrom(req));
        std::vector<ScoredMove> ranked = bot.rankMoves(game);

        if (ranked.empty()) {
            res.set_content("{\"ok\":false}", "application/json");
            return;
        }

        std::string out = "{";
        out += quoted("ok") + ":true,";
        out += quoted("move") + ":" + quoted(moveName(ranked[0].move)) + ",";
        out += quoted("score") + ":" + std::to_string(ranked[0].score);
        out += "}";

        res.set_content(out, "application/json");
    });

    // Start again.
    server.Post("/reset", [](const httplib::Request&, httplib::Response& res) {
        game.reset();
        res.set_content(stateToJson(), "application/json");
    });

    std::cout << "\n";
    std::cout << "  ================================\n";
    std::cout << "      Terminal Chess Engine  (web)\n";
    std::cout << "  ================================\n\n";
    std::cout << "  Open this in your browser:   http://localhost:8080\n";
    std::cout << "  On your phone, same WiFi:    http://<this-pc-ip>:8080\n\n";
    std::cout << "  Press Ctrl+C here to stop the server.\n\n";

    // 0.0.0.0 means other devices on the same network can reach it too.
    server.listen("0.0.0.0", 8080);
    return 0;
}
