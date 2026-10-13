// ============================================================
//  OWNER : Member D
//  LAYER : Interface layer
//  FILE  : src/InputParser.h
//
//  Only Member D edits this file. Ask D for changes.
// ============================================================

#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <string>
#include "Move.h"

//
// Turns what the player typed into a Move.
class InputParser {
public:
    // Reads coordinate notation like "e2e4" or "e2 e4".
    // On success it fills in m and returns true. On malformed input it
    // returns false and leaves m alone.
    static bool parse(const std::string& text, Move& m);

    // Turns a square back into text, so "e4" can be printed in messages.
    static std::string positionToText(Position p);
};

#endif
