#pragma once
#include "GameBoard.h"

class GameBoardData : public BoardData
{
public:
	GameBoardData(int player) : _player(player) {}
	char charAt(Coordinate c) const override;

private:
	GameBoard _board;
	int _player;
};
