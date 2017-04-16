#pragma once

#include "PlayerBase.h"

class PlayerSmart : public PlayerBase
{
public:
	PlayerSmart() : PlayerBase()
	{};
	~PlayerSmart() {};

	/* When I want to make a move on opponent's board*/
	std::pair<int, int> attack() override;

private:
	
};
