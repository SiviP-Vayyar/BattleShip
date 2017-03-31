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

const char** GameBoard::getBoard()
{
	//todo: Having trouble with const char**
	const char** a;
	return a;
}

// TODO: make sure GameBoard::attack can handle ATTACK_END and return AttackResult::Miss
/*update the board to reflect an attack, and notify on the result*/
AttackResult GameBoard::attack(std::pair<int, int> attackPosition)
{
	// ignore ATTACK_END
	if (attackPosition == ATTACK_END)
		return AttackResult::Miss;

	// TODO: implement
	return AttackResult();
}

/*calculate how well the opponent scored on this board at the end of the game*/
int GameBoard::getScore()
{
	// TODO: implemet
	return 0;
}

char& GameBoard::operator()(int row, int col) const
{
	return _board[row - 1][col - 1];
}
