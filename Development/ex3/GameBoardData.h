#pragma once
#include "GameBoard.h"

class GameBoardData : public BoardData
{
public:
	GameBoardData(int player, const GameBoard& board) : _board(board), _player(player)
	{
		_rows = board.rows();
		_cols = board.cols();
		_depth = board.depth();
	}
	char charAt(Coordinate c) const override { return GameBoard::isPlayerShip(_player, _board(c)) ? _board(c) : EMPTY; };

private:
	const GameBoard& _board;
	int _player;
};
