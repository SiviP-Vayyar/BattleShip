#pragma once

#include "PlayerBase.h"

class PlayerNaive : public PlayerBase
{
public:
	PlayerNaive() : PlayerBase() {};
	~PlayerNaive() = default;

	/* When I want to make a move on opponent's board*/
	std::pair<int, int> attack() override;

	/* Called once to allow init from files if needed returns whether the init succeeded or failed*/
	bool init(const std::string& path) override;

private:

	std::vector<std::pair<int, int>> _possibleMoves;
	std::vector<std::pair<int, int>>::iterator _possibleMovesIterator;
};
