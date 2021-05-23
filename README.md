# TerminalChess
C++ script to play a simple game of chess within the terminal, using a text-only display.

The user simply enters a move using the syntax described next to the board. An algorithm tests every possible move
within the board, and filters out the illegal ones. If the user's chosen move is in the list of legal moves, the
move is executed. Otherwise, the user must reenter a different move. Every turn, the current board is checked for
checkmate (no possible moves and king under attack) and stalemate (no possible moves and king not under attack).

terminalChessUnix.cpp includes enhanced display features for unix operating systems. Unicode chess characters and
escape sequences are used to make the display more colorful and easier to interpret. terminalChessUniversal.cpp
functions on all operating systems, but includes a raw text-only display.
