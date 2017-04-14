#include "GameBoard.h"

#include "Player.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cctype>
#include <string>

GameBoard::GameBoard(const std::string& path) : GameBoard()
{
	GameBoard& thisBoard = *this;

	//allocate local empty board for setBoard
	char ** emptyBoard = newEmptyRawBoard(BOARD_ROWS, BOARD_COLS);

	//call setBoard to allocate a board
	setBoard(emptyBoard, BOARD_ROWS, BOARD_COLS);

	//delete local empty board
	deleteRawBoard(emptyBoard, BOARD_ROWS, BOARD_COLS);

	//start reading from file
	std::ifstream inputFileStream(path);
	std::string line;

	int linesReadCount = 1;
	int charInLineCount;
	while (std::getline(inputFileStream, line) && linesReadCount <= BOARD_ROWS )
	{
		charInLineCount = 1;
		for (char pieceChar : line)
		{
			thisBoard(linesReadCount, charInLineCount) = isShip(pieceChar) ? pieceChar : EMPTY;
			charInLineCount++;
		}
		//if not a full line - the board is already filled with EMPTY slots
		linesReadCount++;
	}
	//if under BOARD_ROWS lines - the board is already filled with EMPTY slots
	inputFileStream.close();
}

GameBoard::~GameBoard()
{
	deleteRawBoard(_board, _rows, _cols);
}

/*Caller of this method if resposible for allocation - must call deleteRawBoard()*/
char** GameBoard::newEmptyRawBoard(int rows, int cols)
{
	char** newBoard = new char*[rows]; // Local empty board allocation
	for(int row = 0; row < rows; row++) // Copy board
	{
		newBoard[row] = new char[cols];
		for(int col = 0; col < cols; col++)
		{
			newBoard[row][col] = EMPTY;
		}
	}
	return newBoard;
}

void GameBoard::deleteRawBoard(char** board, int rows, int cols)
{
	if (board == nullptr)
	{
		return;
	}

	for (int i = 0; i < rows; i++)
	{
		delete[] board[i];
	}

	delete[] board;
}

void GameBoard::setBoard(const char* const* board, int numRows, int numCols)
{
	deleteRawBoard(_board, _rows, _cols);
	
	_rows = numRows;
	_cols = numCols;
	_isSet = true;
	_board = new char*[numRows];

	for (int row = 0; row < _rows; row++)
	{
		_board[row] = new char[numCols];
		for (int col = 0; col < _cols; col++)
		{
			_board[row][col] = board[row][col];
		}
	}
}

/*return a new raw board*/
/*@post: the returned board is dynamically allocated and must be freed*/
char** GameBoard::getBoard() const //ZOHAR + SIVAN
{
	char** board = new char*[_rows];

	for (int row = 0; row < _rows; row++)
	{
		board[row] = new char[_cols];
		for (int col = 0; col < _cols; col++)
		{
			board[row][col] = _board[row][col];
		}
	}

	return board;
}

/*return a new raw board with only the player's ships*/
/*@post: the returned boad is dynamically allocated and must be freed*/
char** GameBoard::getBoardForPlayer(int player) const
{
	char** board = new char*[_rows];

	for (int row = 0; row < _rows; row++)
	{
		board[row] = new char[_cols];
		for (int col = 0; col < _cols; col++)
		{
			char piece = _board[row][col];
			board[row][col] = (playerShipType(player, piece) == piece ? piece : EMPTY);
		}
	}
	return board;
}

/*update the board to reflect an attack, and notify on the result and what was hit*/
std::pair<AttackResult, char> GameBoard::attack(std::pair<int, int> attackPosition)
{
	GameBoard& thisBoard = *this;

	// ignore ATTACK_END
	if (attackPosition == ATTACK_END)
		return std::make_pair(AttackResult::Miss, EMPTY);

	int row = attackPosition.first, col = attackPosition.second;
	char piece = thisBoard(row, col);
	
	if (isShip(piece))
	{
		// mark hit on the board
		thisBoard(row, col) = SHIP_HIT;

		// determine if just hit or sink, and add to score if needed
		if (isShipSunk(row, col))
		{
			return std::make_pair(AttackResult::Sink, piece);
		}
		return std::make_pair(AttackResult::Hit, piece);
	}
	return std::make_pair(AttackResult::Miss, piece);
}

bool GameBoard::isShip(char piece)
{
	std::vector<char> shipTypes = { RUBBER, MISSILE, SUB, DESTROYER };
	for (char shipType : shipTypes)
	{
		if (playerShipType(PLAYER_A, shipType) == piece ||
			playerShipType(PLAYER_B, shipType) == piece)
		{
			return true;
		}
	}
	return false;
}

char GameBoard::playerShipType(int player, char typeDef)
{
	return char(player == PLAYER_A ? toupper(typeDef) : tolower(typeDef));
}

// given a position on a ship, explore all directions to determine if sunk
/*@pre: assume (row,col) is some position on a ship*/
bool GameBoard::isShipSunk(int row, int col)
{
	GameBoard& thisBoard = *this;
	auto increments = { std::pair<int,int>(1,0), std::pair<int,int>(-1,0), std::pair<int,int>(0,1), std::pair<int,int>(0,-1) };
	
	// go in all 4 directions from starting position and try to find a piece on the ship that is not hit
	for (auto increment : increments)
	{
		int rowInc = increment.first, colInc = increment.second;
		for (int r=row, c=col ; isInBoard(r,c) ; r+=rowInc, c+=colInc)
		{
			char piece = thisBoard(r, c);
			
			// if this piece was hit - continue searching
			if (piece == SHIP_HIT)
			{
				continue;
			}
			// if this piece is not part of the ship - stop searching in this direction
			if (!isShip(piece))
			{
				break;
			}
			// if we reached this point, this piece of the ship is intact
			return false;
		}
	}
	// seaerched in all directions for a piece of the ship that was not yet hit - and failed
	return true;
}

/*returns the width and length of the ship in position (row,col)*/
/*@Post: May return an underestimate of the dims, but when covering the entire ship, we will get at
 *least one the will catch an illegal ship */
std::pair<int, int> GameBoard::getShipDimensions(const std::set<std::pair<int, int>>& coords) const
{
	int rowMin, rowMax, colMin, colMax;
	rowMin = colMin = _cols + 1;
	rowMax = colMax = 0;
	
	for (auto pos : coords)
	{
		rowMin = std::min(rowMin, pos.first);
		colMin = std::min(colMin, pos.second);
		rowMax = std::max(rowMax, pos.first);
		colMax = std::max(colMax, pos.second);
	}

	return std::make_pair(rowMax - rowMin + 1, colMax - colMin + 1);
}

/*recursively gather all coordinates of the given ship*/
void GameBoard::getShipCoordinates(int row, int col, std::set<std::pair<int, int>>& coords) const
{
	const GameBoard& thisBoard = *this;

	// if this position is already in the set - stop recursion
	if (coords.find(std::make_pair(row,col)) != coords.end())
	{
		return;
	}

	//insert coordinate to set
	coords.insert(std::make_pair(row, col));

	//continue to neighbors
	for (auto neighbor : getAdjacentCoordinatesAsVector(row, col))
	{
		// if same type - continue recursion
		int r = neighbor.first;
		int c = neighbor.second;
		if (thisBoard(r, c) == thisBoard(row, col))
		{
			getShipCoordinates(r, c, coords);
		}
	}
}

/*returns: legalShipsCount, illegalShips*/
std::pair<int, std::set<char>> GameBoard::analyseShips(int player)
{
	GameBoard& thisBoard = *this;
	int legalShipsCount = 0;
	std::set<char> illegalShips;

	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			// skips pieces that are not this player's ship type
			char piece = thisBoard(row, col);
			if(!isShip(piece) || playerShipType(player, piece) != piece)
			{
				continue;
			}

			std::set<std::pair<int, int>> coords;
			getShipCoordinates(row, col, coords);
			auto dim = getShipDimensions(coords);
			int size = getShipLength(piece);

			// a legal ship must be of the right size and shape ("narrow")
			if (size != dim.first*dim.second || (dim.first != 1 && dim.second != 1))
			{
				illegalShips.insert(piece);
			}
			// else - a legal ship
			else
			{
				legalShipsCount++;
			}

			// clear ship from board
			for (auto pos : coords)
			{
				thisBoard(pos.first, pos.second) = EMPTY;
			}
		}
	}
	return std::make_pair(legalShipsCount, illegalShips);
}


/*@return: false iff there are no adjacent ships on the board*/
bool GameBoard::isAdjacent() const
{
	const GameBoard& thisBoard = *this;

	for (int row = 1 ; row <= _rows ; row++)
	{
		for (int col = 1 ; col <= _cols ; col++)
		{
			char center = thisBoard(row, col);
			if (!isShip(center))
			{
				continue;
			}				

			// for every piece on the board that belongs to a ship, check the surrounding pieces
			auto surroudingPositions = getAdjacentCoordinatesAsVector(row, col);
			for (auto position : surroudingPositions)
			{
				// if the adjacent piece is not a ship - it's good
				char adjacentPiece = thisBoard(position.first, position.second);
				if (!isShip(adjacentPiece))
				{
					continue;
				}

				// if center and adjacentPiece are of different ships or players - it's bad
				if (center != adjacentPiece)
				{
					return true;
				}
			}			
		}
	}
	return false;
}

int GameBoard::getShipScore(char piece)
{
	int size = 4;
	char shipTypes[] = { RUBBER, MISSILE, SUB, DESTROYER };
	int shipScores[] = { RUBBER_SCORE, MISSILE_SCORE, SUB_SCORE, DESTROYER_SCORE };

	for (int i=0 ; i < size ; i++)
	{
		auto shipType = shipTypes[i];
		if (playerShipType(PLAYER_A, shipType) == piece ||
			playerShipType(PLAYER_B, shipType) == piece)
		{
			return shipScores[i];
		}
	}
	return 0;
}

int GameBoard::getShipLength(char piece)
{
	int size = 4;
	char shipTypes[] = { RUBBER, MISSILE, SUB, DESTROYER };
	int shipLengths[] = { RUBBER_LEN, MISSILE_LEN, SUB_LEN, DESTROYER_LEN };

	for (int i = 0; i < size; i++)
	{
		auto shipType = shipTypes[i];
		if (playerShipType(PLAYER_A, shipType) == piece ||
			playerShipType(PLAYER_B, shipType) == piece)
		{
			return shipLengths[i];
		}
	}
	return 0;
}

char& GameBoard::operator()(int row, int col)
{
	return _board[row - 1][col - 1];
}

char GameBoard::operator()(int row, int col) const
{
	return _board[row - 1][col - 1];
}

GameBoard& GameBoard::operator=(const GameBoard& other)
{
	setBoard(other._board, other._rows, other._cols);
	return *this;
};

/*return a vector of all valid surrounding Coordinates*/
std::vector<std::pair<int, int>> GameBoard::getSurroundingCoordinatesAsVector(int row, int col) const
{
	std::vector<std::pair<int, int>> surroundingCoordinates;

	//iterate through all surrounding Coordinates and if valid -> add to return vector
	for (int tempRow = row - 1; tempRow <= row + 1; ++tempRow)
	{
		for (int tempCol = col - 1; tempCol <= col + 1; ++tempCol)
		{
			if (tempCol == col && tempRow == row)
			{
				continue;
			}
			if (isInBoard(tempRow, tempCol))
			{
				surroundingCoordinates.push_back(std::pair<int, int>(tempRow, tempCol));
			}
		}
	}
	return surroundingCoordinates;
}

/*returns a vector of all adjacent Coordinates (does not include diagonal Coordinates)*/
std::vector<std::pair<int, int>> GameBoard::getAdjacentCoordinatesAsVector(int row, int col) const
{
	std::vector<std::pair<int, int>> adjacentCoordinates;

	auto increments = { std::pair<int,int>(1,0), std::pair<int,int>(-1,0), std::pair<int,int>(0,1), std::pair<int,int>(0,-1) };
	for (auto inc : increments)
	{
		int r = row + inc.first, c = col + inc.second;
		if (isInBoard(r, c))
		{
			adjacentCoordinates.push_back(std::pair<int, int>(r, c));
		}
	}
	return adjacentCoordinates;
}
