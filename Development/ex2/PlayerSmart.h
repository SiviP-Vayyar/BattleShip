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

	/* Called once to allow init from files if needed returns whether the init succeeded or failed*/
	bool init(const std::string& path) override { return true; }

private:
	std::vector<std::pair<int, int>> getAllPositions(char type);
	std::pair<int, int> selectAttackPositionFromEmptyPositions(const std::vector<std::pair<int, int>>& emptyPositions);
};
