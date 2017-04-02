#include "GameBoard.h"
#include "Player.h"

GameBoard::GameBoard(const std::string& path) : GameBoard()
{
	// TODO: implement
	// read numRows, numCols and board matrix from file, then call setBoard
	throw std::exception("Not Implemented");
}

GameBoard::~GameBoard()
{
	deleteRawBoard(_board, _rows, _cols);
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

	for(int row = 0; row < _rows; row++)
	{
		_board[row] = new char[numCols];
		for (int col = 0; col < _rows; col++)
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
		for (int col = 0; col < _rows; col++)
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
		for (int col = 0; col < _rows; col++)
		{
			char piece = _board[row][col];
			board[row][col] = (playerShipType(player, piece) == piece ? piece : EMPTY);
		}
	}

	return board;
}

/*update the board to reflect an attack, and notify on the result*/
AttackResult GameBoard::attack(std::pair<int, int> attackPosition)
{
	GameBoard& thisBoard = *this;

	// ignore ATTACK_END
	if (attackPosition == ATTACK_END)
		return AttackResult::Miss;

	int row = attackPosition.first, col = attackPosition.second;
	char piece = thisBoard(row, col); //TODO: make sure this a COPY, not a reference
	
	if (isShip(piece))
	{
		// mark hit on the board
		thisBoard(row, col) = SHIP_HIT;

		// determine if just hit or sink, and add to score if needed
		if (isShipSunk(row, col))
		{
			_score += getShipScore(piece);
			return AttackResult::Sink;
		}
		return AttackResult::Hit;
	}

	return AttackResult::Miss;
}

bool GameBoard::isShip(char piece)
{
	std::vector<char> shipTypes = { RUBBER, MISSILE, SUB, DESTROYER };
	for (char shipType : shipTypes)
	{
		if (playerShipType(PLAYER_A, shipType) == piece ||
			playerShipType(PLAYER_B, shipType) == piece)
			return true;
	}

	return false;
}

inline bool GameBoard::isInBoard(int row, int col) const
{
	return 0 < row && row <= _rows && 0 < col && col <= _cols;
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
				continue;
			
			// if this piece is not part of the ship - stop searching in this direction
			if (!isShip(piece))
				break;
			
			// if we reached this point, this piece of the ship is intact
			return false;
		}
	}

	// seaerched in all directions for a piece of the ship that was not yet hit - and failed
	return true;
}

/*finds ships with illegal shape or size for player, and appends them to the vector*/
void GameBoard::getIllegalShips(int player, std::vector<char>& illegalShips) const
{
	// TODO: implement
	throw std::exception("Not implemented");
}

/*count ships of legal shape and size for player*/
int GameBoard::countShips(int player) const
{
	// TODO: implement
	throw std::exception("Not implemented");
}

/*@return: false iff there are no adjacent ships on the board*/
bool GameBoard::isAdjacent() const
{
	// TODO: implement
	throw std::exception("Not implemented");
}

int GameBoard::getShipScore(char piece)
{
	int size = 4;
	char shipTypes[] = { RUBBER, MISSILE, SUB, DESTROYER };
	int shipScores[] = { RUBBER_SCORE, MISSILE_SCORE, SUB_SCORE, DESTROYER_SCORE };

	for (int i=0 ; i<size ; i++)
	{
		auto shipType = shipTypes[i];
		if (playerShipType(PLAYER_A, shipType) == piece ||
			playerShipType(PLAYER_B, shipType) == piece)
			return shipScores[i];
	}

	return 0;
}

char& GameBoard::operator()(int row, int col) const
{
	return _board[row - 1][col - 1];
}

/*return a vector of all valid surrounding cordinates*/
std::vector<std::pair<int, int>> GameBoard::getSurroundingCordinatesAsVector(int row, int col) const
{
	std::vector<std::pair<int, int>> surroundingCordinates;

	//iterate through all surrounding cordinates and if valid -> add to return vector
	for (int tempRow = row - 1; tempRow <= row + 1; ++tempRow) {
		for (int tempCol = col - 1; tempCol <= col + 1; ++tempCol) {
			if (tempCol == col && tempRow == row)
			{
				continue;
			}
			if (isInBoard(tempRow, tempCol))
			{
				surroundingCordinates.push_back(std::pair<int, int>(tempRow, tempCol));
			}
		}
	}

	return surroundingCordinates;
	
}

/*returns a vector of all adjacent cordinates (does not include diagonal cordinates)*/
std::vector<std::pair<int, int>> GameBoard::getAdjacentCordinatesAsVector(int row, int col) const
{
	std::vector<std::pair<int, int>> adjacentCordinates;

	if (isInBoard(row - 1, col -1))
	{
		adjacentCordinates.push_back(std::pair<int, int>(row - 1, col - 1));
	}
	if (isInBoard(row + 1, col - 1))
	{
		adjacentCordinates.push_back(std::pair<int, int>(row + 1, col - 1));
	}
	if (isInBoard(row - 1, col + 1))
	{
		adjacentCordinates.push_back(std::pair<int, int>(row - 1, col + 1));
	}
	if (isInBoard(row + 1, col + 1))
	{
		adjacentCordinates.push_back(std::pair<int, int>(row + 1, col + 1));
	}

	return adjacentCordinates;
}