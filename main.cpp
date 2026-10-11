// ============================================================
//  OWNER : Member D
//  LAYER : Interface layer
//  FILE  : src/main.cpp
//
//  Only Member D edits this file. Ask D for changes.
// ============================================================

#include <iostream>
#include <string>
#include "GameState.h"
#include "Display.h"
#include "InputParser.h"

// Cuts the spaces off both ends and makes it lowercase, so the player can
// type "Quit" or " quit " and still be understood.
static std::string tidyCommand(const std::string& line) {
    int start = 0;
    int end = (int)line.size() - 1;

    while (start <= end && (line[start] == ' ' || line[start] == '\t')) {
        start++;
    }
    while (end >= start && (line[end] == ' ' || line[end] == '\t' ||
                            line[end] == '\r' || line[end] == '\n')) {
        end--;
    }

    std::string out = "";
    for (int i = start; i <= end; i++) {
        char ch = line[i];
        if (ch >= 'A' && ch <= 'Z') {
            ch = ch + ('a' - 'A');
        }
        out = out + ch;
    }
    return out;
}

int main() {
    GameState game;

    std::cout << "\n";
    std::cout << "  ================================\n";
    std::cout << "        Terminal Chess Engine\n";
    std::cout << "  ================================\n";
    Display::showHelp();

    while (true) {
        Display::render(game.getBoard());
        Display::showStatus(game);

        // Say something if the position is special, and stop if it is finished.
        GameStatus status = game.evaluateStatus();
        std::string note = Display::statusMessage(status, game.getTurn());
        if (note != "") {
            std::cout << "  " << note << "\n";
        }
        if (status == CHECKMATE || status == STALEMATE) {
            std::cout << "  Game over.\n\n";
            break;
        }

        std::cout << "\n  Your move: ";

        std::string line;
        if (!std::getline(std::cin, line)) {
            // No more input, for instance when the game is being fed from a file.
            std::cout << "\n  End of input. Goodbye.\n";
            break;
        }

        std::string command = tidyCommand(line);

        if (command == "") {
            continue;
        }
        if (command == "quit" || command == "exit") {
            std::cout << "  Goodbye.\n\n";
            break;
        }
        if (command == "help") {
            Display::showHelp();
            continue;
        }
        if (command == "board") {
            continue;
        }
        if (command == "history") {
            Display::showHistory();
            continue;
        }
        if (command == "save" || command == "load") {
            // TODO (remaining work): saveGame() and loadGame(), writing the
            // game to a plain text file and reading it back.
            std::cout << "  Saving and loading are not finished yet.\n";
            continue;
        }

        Move m;
        if (!InputParser::parse(command, m)) {
            std::cout << "  I did not understand that. Moves look like e2e4.";
            std::cout << " Type help for the commands.\n";
            continue;
        }

        // Look at the piece before the move, so we can describe what happened
        // afterwards when the squares have already changed.
        const Piece* mover = game.getBoard().getPieceAt(m.from);
        std::string moverName = "Piece";
        bool isCapture = false;
        if (mover != nullptr) {
            moverName = mover->getName();
            const Piece* target = game.getBoard().getPieceAt(m.to);
            if (target != nullptr && target->getColor() != mover->getColor()) {
                isCapture = true;
            }
        }

        MoveResult result = game.makeMove(m);

        if (result != MOVE_OK) {
            std::cout << "  " << Display::moveResultMessage(result) << "\n";
            continue;
        }

        std::cout << "  " << moverName << " ";
        if (isCapture) {
            std::cout << "takes on ";
        } else {
            std::cout << "to ";
        }
        std::cout << InputParser::positionToText(m.to) << ".\n";
    }

    return 0;
}
