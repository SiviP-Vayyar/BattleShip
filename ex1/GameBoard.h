#pragma once

class GameBoard
{
public:
	GameBoard() : _board(nullptr), _rows(0), _cols(0), _isSet(false){}
	~GameBoard();

	void setBoard(const char** board, int numRows, int numCols);
	int rows() const { return _rows; }
	int cols() const { return _cols; }
	bool isSet() const { return _isSet; }
	const char** getBoard();

	/*Using 1-based matrix call on vector*/
	char& operator()(int row, int col) const;
	
private:
	char** _board;
	int _rows;
	int _cols;
	bool _isSet;
};
