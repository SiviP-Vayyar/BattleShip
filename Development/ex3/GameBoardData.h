#pragma once
#include "GameBoard.h"

class GameBoardData : public BoardData
{
public:
	GameBoardData(int player, const GameBoard& board) : _board(board), _player(player) {}
	char charAt(Coordinate c) const override;

private:
	const GameBoard& _board;
	int _player;
};
