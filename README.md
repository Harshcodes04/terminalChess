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

### The coach and the computer opponent

`src/Bot.h` and `src/Bot.cpp` add a computer player that doubles as a coach.
It knows no chess rules of its own: it asks `GameState` for the legal moves,
plays them on a copy of the game and looks a few moves ahead. The copy is the
same deep copy the rules layer already uses to test for check, so the bot only
exists because `Board` has a real copy constructor and `Piece` has `clone()`.

How it decides:

- **Counting the position.** Each piece is worth the usual amount, a pawn 100
  and a queen 900. Knights and bishops get a bonus near the middle of the board
  and pawns get one for advancing.
- **Looking ahead.** Minimax with alpha-beta pruning. White tries to push the
  score up and Black tries to push it down, and pruning throws away lines the
  other player would never allow.

Looking three moves ahead takes about 0.05 seconds and four takes about half a
second, which is why four is the deepest setting offered.

Judging a move costs nothing extra. Scoring every legal move before the player
commits gives both the best move and the player's move from the same search,
and the gap between them is the verdict:

| Pawns lost | Verdict |
| --- | --- |
| none | Best move |
| under 0.3 | Good |
| 0.3 to 1.0 | Inaccuracy |
| 1.0 to 2.0 | Mistake |
| over 2.0 | Blunder |

When the move was worse than Good the board outlines the move that should have
been played. The computer can also take a side outright, and there is a hint
button that shows a good move without playing it.

## The three special chess rules

All three are now implemented, so the game follows the full rules of chess.

**Castling.** Type the king moving two squares, so `e1g1` for the short side
and `e1c1` for the long side. The rook jumps over by itself. The king checks
the plain conditions: neither it nor the rook has moved, and the squares
between them are empty. The two conditions it cannot check on its own, that
the king is not in check and does not pass over an attacked square, are
handled by the rules layer, because they are questions about the whole game
rather than about geometry.

**En passant.** When a pawn jumps two squares past an enemy pawn, the board
remembers the square it skipped. An enemy pawn may step diagonally into that
square on the very next move, and the board removes the pawn that jumped. The
chance lasts one move only, because the square is cleared as soon as anything
else is played. The skipped square is stored on the board rather than kept as
history, since it is part of the position.

**Promotion.** A pawn reaching the far row turns into another piece. In the
terminal, add the letter to the move: `a7a8q` for a queen, or `n`, `r`, `b`.
In the browser a small window appears with the four pieces to choose from, and
with voice you can say the piece name along with the move. Leaving the choice
out gives a queen.

### Which browsers voice works in

Voice needs the browser's own speech service, and not every browser has one.

| Browser | Voice | Why |
| --- | --- | --- |
| Chrome | works | has Google's speech service |
| Edge | works | has its own speech service |
| Brave | does not work | Brave removes the speech service on purpose. The page detects this and says so |
| Firefox | does not work | no speech recognition at all |
| A page opened inside another app's window | does not work | built on Chrome but carries no key for the service |

The address also matters. Browsers only allow the microphone on `localhost`
or on `https`, so voice is silent when the page is opened from a phone over
a plain address such as `http://192.168.1.5:8080`. Everything else on the
page works there normally.

The Check voice setup button in the Voice panel tests each of these one at a
time and reports which one is the problem.
