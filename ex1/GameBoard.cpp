#include "GameBoard.h"
#include "Player.h"

GameBoard::~GameBoard()
{
	for(int i = 0; i < _rows; i++)
	{
		delete[] _board[i];
	}

	delete [] _board;
}

void GameBoard::setBoard(const char** board, int numRows, int numCols)
{
	_rows = numRows;
	_cols = numCols;
	_isSet = true;
	_board = new char*[numRows];

	for(int i = 0; i < _rows; i++)
	{
		_board[i] = new char[numCols];
	}

	for(int row = 0; row < _rows; row++)
	{
		for(int col = 0; col < _rows; col++)
		{
			_board[row][col] = board[row][col];
		}
	}
}

const char** GameBoard::getBoard() //ZOHAR + SIVAN
{
	//todo: Having trouble with const char**
	const char** a;
	return a;
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
