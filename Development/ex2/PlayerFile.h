#pragma once

#include "PlayerBase.h"

class PlayerFile : public PlayerBase
{
public:
	PlayerFile() : PlayerBase()
	{};
	~PlayerFile() {};

	/* When I want to make a move on opponent's board*/
	std::pair<int, int> attack() override;

	/*Hard coded algorithm*/
	void SetMoves(std::vector<std::pair<int, int>> moves);

private:
	std::vector<std::pair<int,int>> _myMoves;
	std::vector<std::pair<int, int>>::iterator _movesIterator;
};
