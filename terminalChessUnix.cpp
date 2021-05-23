#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>
using namespace std;

class Piece {
  public:
    Piece();
    Piece(string userColor, char pieceType);
    string getDisplayCode();
    char getType();
    char getColor();
    bool getHasMoved();
    void clear();
    void moved();
  private:
    bool hasMoved;
    char color;
    char type;
    string displayCode;
};

Piece::Piece() {
  color = '-';
  type = '-';
  displayCode = "-";
  hasMoved = true; // necessary for efficiency in castling detection
}

Piece::Piece(string userColor, char pieceType) {
  hasMoved = false;
  type = pieceType;
  pieceType = toupper(pieceType);
  color = tolower(userColor.at(0));

  if (userColor == "white") {  // establishing color
    displayCode = "\e[38;5;15m";  // changes background color in unix terminal (for unix os)
    //displayCode = " w";              // for non-unix os
  } else if (userColor == "black") {
    displayCode = "\e[38;5;232m"; // changes background color in unix terminal (for unix os)
    //displayCode = " b";              // for non-unix os
  } else {
    displayCode = "ERROR";
    cout << displayCode << endl;
  }

  displayCode += " "; // for unix os
  //displayCode += pieceType; //non-unix
  //displayCode += " "; //non-unix

      // for unix os:
  if (pieceType == 'K') {
    displayCode += "\u265A ";
  } else if (pieceType == 'Q') {
    displayCode += "\u265B ";
  } else if (pieceType == 'R') {
    displayCode += "\u265C ";
  } else if (pieceType == 'B') {
    displayCode += "\u265D ";
  } else if (pieceType == 'N') {
    displayCode += "\u265E ";
  } else if (pieceType == 'P') {
    displayCode += "\u265F ";
  }
}

string Piece::getDisplayCode() {
  return displayCode;
}

char Piece::getType() {
  return type;
}

char Piece::getColor() {
  return color;
}

bool Piece::getHasMoved() {
  return hasMoved;
}

void Piece::clear() {
  hasMoved = true; // necessary for efficienty in castling detection
  color = '-';
  type = '-';
  displayCode = "-";
}

void Piece::moved() {
  hasMoved = true;
}

const int ROWS = 8;
const int COLUMNS = 8;
const string whiteBG = "\e[48;5;7m";    // changes bg/fg color in unix os
const string blackBG = "\e[48;5;28m";
const string whiteFG = "\e[38;5;15m";
const string blackFG = "\e[38;5;232m";
const string titleFG = "\e[38;5;28m";
const string reset = "\e[0m";
const string emptyStr = "   ";
// const string whiteBG = "";    // changes bg/fg color in non-unix os
// const string blackBG = "";
// const string whiteFG = "";
// const string blackFG = "";
// const string reset = "";
// const string emptyStr = "   ";
// const string titleFG = "";

int turn = 0;
bool noMoves = false;

void displayBoard(Piece board[ROWS][COLUMNS], bool inCheck, bool inStalemate, string winner);
string toMoveCode(string userCode);
string toMoveCode(int rowFrom, int columnFrom, char pieceChar, int rowTo, int columnTo);
void initializeBoard(Piece board[ROWS][COLUMNS]);
string getMove(Piece board[ROWS][COLUMNS]);
void updateBoard(string moveCode, Piece board[ROWS][COLUMNS]);
bool isValidSyntax(string userCode);
bool isLegalMove(vector<string>& friendlyMoves, string moveCode, Piece board[ROWS][COLUMNS]);
void generatePotentialMoves(char color, vector<string>& moves, Piece board[ROWS][COLUMNS]);
bool underAttack(int row, int column, char color, Piece board[ROWS][COLUMNS]);
bool validCastle(char color, string castleSide, Piece board[ROWS][COLUMNS]);
void removeChecks(vector<string>& moves, char color, Piece board[ROWS][COLUMNS]);

int main() {
  Piece masterBoard[ROWS][COLUMNS];
  initializeBoard(masterBoard);
  displayBoard(masterBoard, false, false, "-");

  while (!noMoves) {
    updateBoard(getMove(masterBoard), masterBoard);
    displayBoard(masterBoard, false, false, "-");
    if (noMoves) {
      char currColor;
      int kingRow, kingColumn;
      if (turn % 2 == 0) {
        currColor = 'w';
      } else {
        currColor = 'b';
      }
      for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
          if (masterBoard[i][j].getType() == 'K' && masterBoard[i][j].getColor() == currColor) {
            kingRow = i;
            kingColumn = j;
          }
        }
      }
      if (underAttack(kingRow, kingColumn, currColor, masterBoard)) {
        if (currColor == 'w') {
          displayBoard(masterBoard, true, false, "BLACK");
        } else {
          displayBoard(masterBoard, true, false, "WHITE");
        }
      } else {
        displayBoard(masterBoard, false, true, "-");
      }
    }
    turn++;
  }
}


string getMove(Piece board[ROWS][COLUMNS]) {
  string message;
  char currColor;
  if (turn % 2 == 0) {
    message = "White to move: ";
    currColor = 'w';
  } else {
    message = "Black to move: ";
    currColor = 'b';
  }

  // Generating possible Moves:
  vector<string> friendlyMoves;
  generatePotentialMoves(currColor, friendlyMoves, board);
  removeChecks(friendlyMoves, currColor, board);
  if (validCastle(currColor, "0-0", board)) { //adding cast
    friendlyMoves.push_back("0-0");
  }
  if (validCastle(currColor, "0-0-0", board)) {
    friendlyMoves.push_back("0-0-0");
  }

  noMoves = true;
  for (int i = 0; i < friendlyMoves.size(); i++) {
    if (friendlyMoves.at(i) != "-") {
      noMoves = false;
    }
  }

  if (noMoves) {
    return "noMoves";
  }

  bool isLegal, isValid;
  string userCode;
  do {
    cout << whiteFG << message;
    cin >> userCode;
    if (userCode != "0-0" && userCode != "0-0-0" && userCode.size() == 5) { // turn piecechar to uppercase if not castle move
      userCode.at(2) = toupper(userCode.at(2));
    }
    isValid = isValidSyntax(userCode);
    if (isValid) {
      isLegal = isLegalMove(friendlyMoves, toMoveCode(userCode), board);
    }
    if (!isValid || !isLegal) {
      displayBoard(board, false, false, "-"); // for aesthetic purposes - keeps board in same place on terminal.
      cout << whiteFG << "Invalid Move. ";
    }
  } while (!isValid || !isLegal);

  return toMoveCode(userCode);
}

//assumes movecode is validsyntax and legal move.
void updateBoard(string moveCode, Piece board[ROWS][COLUMNS]) {
  if (moveCode == "noMoves") {
    cout << "no moves" << endl;
    return;
  }
  if (moveCode != "0-0" && moveCode != "0-0-0") {
    int rowFrom = moveCode.at(0) - 48;
    int columnFrom = moveCode.at(1) - 48;
    int rowTo = moveCode.at(3) - 48;
    int columnTo = moveCode.at(4) - 48;

    board[rowFrom][columnFrom].moved();
    board[rowTo][columnTo] = board[rowFrom][columnFrom];
    board[rowFrom][columnFrom].clear();
  } else {
    if (turn % 2 == 0) {  // white is castling
      board[0][4].moved();
      if (moveCode == "0-0") { //kingside
        board[0][7].moved();
        board[0][5] = board[0][7];
        board[0][7].clear();
        board[0][6] = board[0][4];
        board[0][4].clear();
      } else if (moveCode == "0-0-0") { //queenside
        board[0][0].moved();
        board[0][3] = board[0][0];
        board[0][0].clear();
        board[0][2] = board[0][4];
        board[0][4].clear();
      }
    } else {  // black is castling
      board[7][4].moved();
      if (moveCode == "0-0") { // kingside
        board[7][7].moved();
        board[7][5] = board[7][7];
        board[7][7].clear();
        board[7][6] = board[7][4];
        board[7][4].clear();
      } else if (moveCode == "0-0-0") { //queenside
        board[7][0].moved();
        board[7][3] = board[7][0];
        board[7][0].clear();
        board[7][2] = board[7][4];
        board[7][4].clear();
      }
    }
  }
}

//assumes movecode is validsyntax (move is in board as well)
bool isLegalMove(vector<string>& friendlyMoves, string moveCode, Piece board[ROWS][COLUMNS]) {
  char currColor;
  if (turn % 2 == 0) {
    currColor = 'w';
  } else {
    currColor = 'b';
  }

  for (int i = 0; i < friendlyMoves.size(); i++) {
    if (moveCode == friendlyMoves.at(i)) {
      return true;
    }
  }
  return false;
  // need to generate potential moves, refine to legal moves (including castles), and check if moveCode is in that list of legal moves.
}

bool isValidSyntax(string userCode) {
  if (userCode == "0-0" || userCode == "0-0-0") {
    return true;
  }
  else if (userCode.size() == 5) {
    if (userCode.at(1) >= 49 && userCode.at(1) <= 56 && userCode.at(4) >= 49 && userCode.at(4) <= 56) { // valid rows
      if (userCode.at(0) >= 97 && userCode.at(0) <= 104 && userCode.at(3) >= 97 && userCode.at(3) <= 104) { // valid columns
        if (userCode.at(2) == 'K' || userCode.at(2) == 'Q' || userCode.at(2) == 'R' || userCode.at(2) == 'B' || userCode.at(2) == 'N' || userCode.at(2) == 'P') { // valid piece char
          return true;
        }
      }
    }
  }
  return false;
}

void generatePotentialMoves(char color, vector<string>& moves, Piece board[ROWS][COLUMNS]) {

  char oppoColor = 217 - color;
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      int rowFrom = i;
      int columnFrom = j;

      if (board[i][j].getColor() == color) {
        if (board[i][j].getType() == 'P') { // pawns
          int dr = 1 - 2 * (color < 108); // 1 for white, -1 for black (direction of pawn movement)
          int initialPawnRow = 1 + 5 * (color < 108);
          if (board[i + dr][j].getType() != color) { //move one up
            moves.push_back(toMoveCode(i, j, 'P', i + dr, j));
          }
          if (i == initialPawnRow && board[i + dr][j].getType() != color && board[i + 2 * dr][j].getType() != color) { // move two up
            moves.push_back(toMoveCode(i, j, 'P', i + 2 * dr, j));
          }
          for (int k = -1; k < 2; k +=2 ) {
            if (board[i + dr][j + k].getColor() == oppoColor) { //take diagonally
              moves.push_back(toMoveCode(i, j, 'P', i + dr, j + k));
            }
          }
        } else if (board[i][j].getType() == 'R' || board[i][j].getType() == 'B' || board[i][j].getType() == 'Q' || board[i][j].getType() == 'K') { // sliding pieces
          int dr, dc, numDirections;
          if (board[i][j].getType() == 'R' || board[i][j].getType() == 'B') {
            numDirections = 4;
          } else {
            numDirections = 8;
          }
          for (int k = 0; k < numDirections; k++) {
            if (board[i][j].getType() == 'R') { // for k{0, 1, 2, 3} yields: dr{0, -1, 0, 1} and dc{1, 0, -1, 0}. pointing right, down, left, and up.
              dr = (k / 2) - (((k + 1) / 2) % 2);
              dc = 1 + 2 * (k / 3) - k;
            } else if (board[i][j].getType() == 'B') { // for k{0, 1, 2, 3} yields: dr{-1, 1, 1, -1} dc{-1, -1, 1, 1}. pointing down left clockwise to down right
              dr = 1 - 2 * (((k + 2) % 3) / 2);
              dc = 2 * (k / 2) - 1;
            } else {  // for k{0,1,2,3,4,5,6,7} yields dr{1,0,-1,-1,-1,0,1,1} and dc{1,1,1,0,-1,-1,-1,0}. pointing up right clockwise to up.
              dr = (((k + 6) % 7) / 5) - (((k + 1) % 6) / 3);
              dc = (((k + 5) % 8) / 5) - (((k + 1) % 8) / 5);
            }

            int rowTo = i + dr;
            int columnTo = j + dc;

            while (rowTo >= 0 && rowTo <= 7 && columnTo >= 0 && columnTo <= 7 && board[rowTo][columnTo].getColor() != color) { // 'slides' until rook reaches other piece or end of board.
              moves.push_back(toMoveCode(i, j, board[i][j].getType(), rowTo, columnTo));
              if (board[rowTo][columnTo].getColor() == oppoColor || board[i][j].getType() == 'K') {  // stops once reaches an opponent piece. or if piece is king, in which case it stops after one iteration (can only move one in any direction)
                break;
              }
              rowTo += dr;
              columnTo += dc;
            }
          }
        } else if (board[i][j].getType() == 'N') { // knights
          int dr, dc;
          for (int k = 0; k < 8; k++) {
            dr = (1 - (2 * (((2 * k) / 4) % 2))) * ((k + 4) / 4);
            dc = (1 - (2 * (k % 2))) * ((11 - k) / 4);

            int rowTo = i + dr;
            int columnTo = j + dc;
            if (rowTo >= 0 && rowTo <= 7 && columnTo >= 0 && columnTo <= 7 && (board[rowTo][columnTo].getColor() != color)) {
              moves.push_back(toMoveCode(i, j, board[i][j].getType(), rowTo, columnTo));
            }
          }
        }
      }
    }
  }
}

//function to check if a given square is under attack by opponent. COLOR GIVEN IS FRIENDLY COLOR
bool underAttack(int row, int column, char color, Piece board[ROWS][COLUMNS]) {
  char oppoColor = 217 - color;
  vector<string> oppoMoves;
  generatePotentialMoves(oppoColor, oppoMoves, board);
  for (int i = 0; i < oppoMoves.size(); i++) {
    int attackRow = oppoMoves.at(i).at(3) - 48;
    int attackColumn = oppoMoves.at(i).at(4) - 48;
    if (attackRow == row && attackColumn == column) {
      return true;
    }
  }
  return false;
}

//function to check if given color can castle (takes color, string 0-0 or 0-0-0 and board)
bool validCastle(char color, string castleSide, Piece board[ROWS][COLUMNS]) {
  int row, rookColumn, dc; // dc is direction of king
  int kingColumn = 4;
  if (color == 'w') { // white
    row = 0;
  } else if (color == 'b') { // black
    row = 7;
  }
  if (castleSide == "0-0") { // kingside
    rookColumn = 7;
    dc = 1;
  } else if (castleSide == "0-0-0") { // queenside
    rookColumn = 0;
    dc = -1;
  }

  if (!board[row][kingColumn].getHasMoved() && !board[row][rookColumn].getHasMoved()) { // if king and rook haven't moved
    for (int i = kingColumn + dc; i != rookColumn; i += dc) { // checking if pieces in between
      if (board[row][i].getType() != '-') {
        return false;
      }
    }
    for (int i = kingColumn; i != kingColumn + (3 * dc); i += dc) {
      if (underAttack(row, i, color, board)) {
        return false;
      }
    }
  return true;
  }
  return false;
}

void removeChecks(vector<string>& moves, char color, Piece board[ROWS][COLUMNS]) {
  for (int i = 0; i < moves.size(); i++) {
    Piece testBoard[ROWS][COLUMNS]; // resetting testBoard
    for (int x = 0; x < ROWS; x++) {
      for (int y = 0; y < COLUMNS; y++) {
        testBoard[x][y] = board[x][y];
      }
    }

    updateBoard(moves.at(i), testBoard);
    for (int j = 0; j < ROWS; j++) {
      for (int k = 0; k < COLUMNS; k++) {
        if (testBoard[j][k].getType() == 'K') {
          if (underAttack(j, k, color, testBoard)) {
            moves.at(i) = "-";
          }
        }
      }
    }
  }

}
    // For unix os:
void displayBoard(Piece board[ROWS][COLUMNS], bool inCheck, bool inStalemate, string winner) {
  cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

  for (int i = ROWS - 1; i >= 0; i--) {
    cout << whiteFG << " " << i + 1 << "  ";
    for (int j = 0; j <= COLUMNS - 1; j++) {
      string currPiece = board[i][j].getDisplayCode();
      if ((i + j) % 2 == 0) {
        cout << blackBG;
      } else {
        cout << whiteBG;
      }
      if (currPiece != "-") {
        cout << currPiece;
      } else {
        cout << emptyStr;
      }
    }
    if (i == 7) {
      cout << reset << titleFG << "\t\tWelcome to Terminal Chess";
    } else if (i == 6) {
      cout << reset << whiteFG << "\tTo move single piece:";
    } else if (i == 5) {
      cout << reset << whiteFG << "\t  Enter square from, piece, and square to.";
    } else if (i == 4) {
      cout << reset << whiteFG << "\t  Example: a2Pa3";
    } else if (i == 3) {
      cout << reset << whiteFG << "\tTo castle:";
    } else if (i == 2) {
      cout << reset << whiteFG << "\t  Enter 0-0 for kingside, 0-0-0 for queenside.";
    } else if (i == 0 && inCheck) {
      cout << reset << titleFG << "\e[5m" << "\t\tCHECKMATE! " << winner << " WINS!";
    } else if (i == 0 && inStalemate) {
      cout << reset << whiteFG << "\e[5m" << "\t\t Stalemate...";
    }
    cout << reset << endl;
  }
  cout << "    ";
  char column = 'a';
  for (int i = 0; i < 8; i++) {
    cout << " " << whiteFG << column++ << " ";
  }
  cout << endl;
}

  //For non-unix os
// void displayBoard(Piece board[ROWS][COLUMNS], bool inCheck, bool inStalemate, string winner) {
//   cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
//   cout << "    ";
//   for (int i = 0; i < 55; i++) {
//     cout << "_";
//   }
//   cout << endl;
//   for (int i = ROWS - 1; i >= 0; i--) {
//     string message = "";
//     for (int k = 0; k < 3; k++) {
//       if (k == 1) {
//         cout << i + 1 << "  ";
//       } else {
//         cout << "   ";
//       }
//       for (int j = 0; j < 8; j++) {
//         if (k == 0) {
//           cout << "|      ";
//         } else if (k == 1) {
//           cout << "|";
//           if (board[i][j].getDisplayCode() == "-") {
//             cout << "      ";
//           } else {
//             cout << " " << board[i][j].getDisplayCode() << " ";
//           }
//         } else if (k == 2) {
//           cout << "|______";
//         }
//       }
//       cout << "|     ";
//       if (i == 7 && k == 1) {
//         cout << "Welcome to terminal chess!";
//       } else if (i == 6 && k == 0) {
//         cout << "Input syntax:";
//       } else if (i == 6 && k == 1) {
//         cout << "   To move pawn at a2 to a3, enter:";
//       } else if (i == 6 && k == 2) {
//         cout << "      a2pa3";
//       } else if (i == 5 && k == 0) {
//         cout << "Kingside castle: 0-0";
//       } else if (i == 5 && k == 1) {
//         cout << "Queenside castle: 0-0-0";
//       }
//       cout << endl;
//     }
//   }
//   cout << endl << "       a      b      c      d      e      f      g      h" << endl;
//   if (inStalemate) {
//     cout << "Stalemate..." << endl;
//   } else if (inCheck) {
//     cout << "Checkmate!";
//     if (turn == 0) {
//       cout << " Black wins!" << endl;
//     } else {
//       cout << " White wins!" << endl;
//     }
//   }
// }

// works for any user code, including castling. (assumes valid syntax)
string toMoveCode(string userCode) {
  if (userCode != "0-0" && userCode != "0-0-0") { // non castle move
    string moveCode = "";
    moveCode += userCode.at(1) - 1;
    moveCode += userCode.at(0) - 49;
    moveCode += userCode.at(2);
    moveCode += userCode.at(4) - 1;
    moveCode += userCode.at(3) - 49;
    return moveCode;
  } else { //castle move
    return userCode;
  }
}

string toMoveCode(int rowFrom, int columnFrom, char pieceChar, int rowTo, int columnTo) {
  string moveCode = "";
  moveCode += to_string(rowFrom) + to_string(columnFrom);
  moveCode += pieceChar;
  moveCode += to_string(rowTo) + to_string(columnTo);
  return moveCode;
}

void initializeBoard(Piece board[ROWS][COLUMNS]) {
  for (int j = 0; j < 8; j++) {
    board[1][j] = Piece("white", 'P');
    board[6][j] = Piece("black", 'P');
  }
  board[0][0] = Piece("white", 'R');
  board[0][1] = Piece("white", 'N');
  board[0][2] = Piece("white", 'B');
  board[0][3] = Piece("white", 'Q');
  board[0][4] = Piece("white", 'K');
  board[0][5] = Piece("white", 'B');
  board[0][6] = Piece("white", 'N');
  board[0][7] = Piece("white", 'R');

  board[7][0] = Piece("black", 'R');
  board[7][1] = Piece("black", 'N');
  board[7][2] = Piece("black", 'B');
  board[7][3] = Piece("black", 'Q');
  board[7][4] = Piece("black", 'K');
  board[7][5] = Piece("black", 'B');
  board[7][6] = Piece("black", 'N');
  board[7][7] = Piece("black", 'R');
}
