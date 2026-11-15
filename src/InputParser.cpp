

#include "InputParser.h"

// Throws away spaces and tabs and makes letters lowercase, so "E2 E4" and
// "e2e4" both work.
static std::string tidyUp(const std::string& text) {
    std::string out = "";
    for (unsigned int i = 0; i < text.size(); i++) {
        char ch = text[i];
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            continue;
        }
        if (ch >= 'A' && ch <= 'Z') {
            ch = ch + ('a' - 'A');
        }
        out = out + ch;
    }
    return out;
}

bool InputParser::parse(const std::string& text, Move& m) {
    std::string s = tidyUp(text);

    // Four characters for a normal move: file, rank, file, rank. A fifth
    // letter is allowed when a pawn reaches the far side, saying what it
    // should turn into, for example "a7a8q".
    if (s.size() != 4 && s.size() != 5) {
        return false;
    }

    char promoteTo = 0;
    if (s.size() == 5) {
        char w = s[4];
        if (w != 'q' && w != 'r' && w != 'b' && w != 'n') {
            return false;
        }
        promoteTo = w - ('a' - 'A');   // store it as a capital letter
    }

    char fromFile = s[0];
    char fromRank = s[1];
    char toFile = s[2];
    char toRank = s[3];

    if (fromFile < 'a' || fromFile > 'h') {
        return false;
    }
    if (toFile < 'a' || toFile > 'h') {
        return false;
    }
    if (fromRank < '1' || fromRank > '8') {
        return false;
    }
    if (toRank < '1' || toRank > '8') {
        return false;
    }

    // 'a' is column 0 and '1' is row 0.
    m.from = Position(fromRank - '1', fromFile - 'a');
    m.to = Position(toRank - '1', toFile - 'a');
    m.promotion = promoteTo;
    return true;
}

std::string InputParser::positionToText(Position p) {
    if (!isOnBoard(p)) {
        return "??";
    }
    std::string out = "";
    out = out + (char)('a' + p.col);
    out = out + (char)('1' + p.row);
    return out;
}
