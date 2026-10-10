// ============================================================
//  OWNER : Member C
//  LAYER : Rules and game state
//  FILE  : src/GameState.cpp
//
//  Only Member C edits this file. Ask C for changes.
// ============================================================

#include "GameState.h"

GameState::GameState() {
    board.setupInitialPosition();
    turn = WHITE;
    moveNumber = 1;
}

Color GameState::getTurn() const {
    return turn;
}

int GameState::getMoveNumber() const {
    return moveNumber;
}

const Board& GameState::getBoard() const {
    return board;
}

