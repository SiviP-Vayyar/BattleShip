#pragma once
#include <vector>

class GameBoard
{
public:
	GameBoard();
	~GameBoard();

	void setBoard(const char** board, int numRows, int numCols);
	int rows() { return _rows; }
	int cols() { return _cols; }
	bool isSet() { return _isSet; }

	/*Using 1-based matrix call on vector*/
	char& operator()(int row, int col);
	
private:
	char** _board;
	int _rows;
	int _cols;
	bool _isSet;
};
