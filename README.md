# Terminal Chess
C++ script to play a 2-player game of chess within the terminal, using a text-only display.

The user enters a move using the syntax described next to the board. An algorithm tests every possible move
within the board, and filters out the illegal ones. If the user's chosen move is in the list of legal moves, the
move is executed. Otherwise, the user must reenter a different move. Every turn, the current board is checked for
checkmate (no possible moves and king under attack) and stalemate (no possible moves and king not under attack).

terminalChessOSX.cpp includes enhanced display features for Mac operating systems. Unicode chess characters and
escape sequences are used to make the display more colorful and easier to interpret. terminalChessUniversal.cpp
functions on all operating systems, but includes a raw text-only display.

Features not yet supported: en passant, pawn promotion.

## Mac Display
![Terminal Chess on Mac](terminal_chess_osx.png)
