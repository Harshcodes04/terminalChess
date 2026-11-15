# Terminal Based Chess Engine

A two player chess game that runs in the terminal. Written in C++ for the
Project Based Learning module, following the module and function allocation
document.

See `PROGRESS.md` for exactly which functions are finished and which are not.

## Building

With make:

    make

Without make, on Windows:

    build.bat

Without make, anywhere else:

    sh build.sh

## Playing

    ./chess

or `chess.exe` on Windows.

Type a move as the square you are moving from followed by the square you are
moving to, with no space, for example `e2e4`. The other commands are `help`,
`board`, `history` and `quit`.

## How the code is laid out

The system is four layers, and each layer only uses the layer beneath it.

    Presentation  ->  Game logic  ->  Board  ->  Pieces

| File | Layer | Member |
| --- | --- | --- |
| `src/Position.h`, `src/Move.h` | shared, frozen in the Week 0 design session | everyone |
| `src/Board.h`, `src/Board.cpp` | the 8x8 grid and all piece memory | A |
| `src/Piece.h`, `src/Piece.cpp` | the abstract base class | B |
| `src/Pieces.h`, `src/Pieces.cpp` | the six concrete pieces | B |
| `src/GameState.h`, `src/GameState.cpp` | legality, check, checkmate, turns | C |
| `src/Display.h`, `src/Display.cpp` | all screen output | D |
| `src/InputParser.h`, `src/InputParser.cpp` | reads what the player typed | D |
| `src/main.cpp` | the game loop | D |

The point of the split is that the layers cannot reach into each other. The
board keeps its grid private, so the only way to change the board is
`movePiece()`. The pieces only answer geometry questions and know nothing
about turns or check. `GameState` owns whose turn it is and nothing outside
can change it. Nothing except Member D's files prints anything, so the rules
can be tested without any screen output.

## The one part worth explaining out loud

Stopping a player from leaving their own king in check is the only place where
the layers have to cooperate, and it is why two functions in the document
exist only for another member's benefit.

`GameState::wouldExposeOwnKing()` cannot just play the move and then undo it,
because undoing a capture means putting back a piece that has already been
deleted. Instead it copies the whole board, plays the move on the copy, and
asks whether the king is attacked on the copy. The real board is never
touched.

That copy has to be a deep copy. If it shared pieces with the real board, the
simulation would move the real pieces around and quietly corrupt the game. So
`Board` has a copy constructor, and because the board only holds `Piece*` and
does not know what kind of piece is on any square, it calls `Piece::clone()`
and lets each piece build its own copy.

## Playing in a browser

As well as the terminal version there is a browser version. The same C++
engine does all the thinking; the browser only draws the board and sends
moves back, so no rule of chess is written twice.

Build it and run it:

    build-web.bat          (or: sh build-web.sh)
    chess-web.exe

Then open http://localhost:8080

On a phone on the same WiFi, run `ipconfig`, find the IPv4 address and open
`http://that-address:8080`.

What it adds over the terminal version:

- Click a piece and every square it may legally move to lights up. A dot
  means an empty square, a ring means a capture. The squares come straight
  from the engine, so a pinned piece correctly shows nothing.
- Voice control. Press Start listening and say a square to pick a piece up,
  then another to put it down, or say the whole move at once such as
  "e2 to e4". The NATO words work too, so "echo two echo four" is the same
  move. "new game" and "flip" also work. Needs Chrome or Edge.
- The king turns red when it is in check.
- A move list, a captured piece tray, and a button to flip the board.

The server is `src/server.cpp` and the page is `web/index.html`. The only
engine change the browser version needed was `GameState::getLegalMoves()`,
which reuses the loop that `hasAnyLegalMove()` already ran.

`third_party/httplib.h` is cpp-httplib, a single header HTTP server under
the MIT licence. It is the only outside code in the project.
