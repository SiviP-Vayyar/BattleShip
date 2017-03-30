#include "GameBoard.h"

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

char& GameBoard::operator()(int row, int col) const
{
	return _board[row - 1][col - 1];
}
