#pragma once
#include "GameBoard.h"

class GameBoardData : public BoardData
{
public:
	GameBoardData(int player, const GameBoard& board) : _board(board), _player(player) {}
	char charAt(Coordinate c) const override { return GameBoard::isPlayerShip(_player, _board(c)) ? _board(c) : ' '; };

private:
	const GameBoard& _board;
	int _player;
};
