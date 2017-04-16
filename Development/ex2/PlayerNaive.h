#pragma once

#include "PlayerBase.h"

class PlayerNaive : public PlayerBase
{
public:
	PlayerNaive() : PlayerBase()
	{};
	~PlayerNaive() {};

	/* When I want to make a move on opponent's board*/
	std::pair<int, int> attack() override;

private:

};
