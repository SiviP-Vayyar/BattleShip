#include "PlayerSmart.h"
#include "GameUtils.h"


std::pair<int, int> PlayerSmart::attack()
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("attack() was called before setBoard was called!");
	}

	// first search for hit on the board and try to expand it
	auto hitPositions = getAllPositions(HIT);
	for (auto hitPosition : hitPositions)
	{
		auto adjPositions = _opponentBoard.getAdjacentCoordinatesAsVector(hitPosition.first, hitPosition.second);
		for (auto adjPosition : adjPositions)
		{
			if (_opponentBoard(adjPosition.first, adjPosition.second) == EMPTY)
			{
				return adjPosition;
			}
		}
	}


	// if no hits were found, collect all empty locations and select from them
	auto emptyPositions = getAllPositions(EMPTY);
	if (!emptyPositions.empty())
	{
		return selectAttackPositionFromEmptyPositions(emptyPositions);
	}

	return ATTACK_END;
}

std::vector<std::pair<int, int>> PlayerSmart::getAllPositions(char type)
{
	std::vector<std::pair<int, int>> hitPositions;

	for (int row = 1 ; row <= _opponentBoard.rows() ; row++)
	{
		for (int col = 1; col <= _opponentBoard.cols(); col++)
		{
			if (_opponentBoard(row, col) == type)
			{
				hitPositions.push_back(std::make_pair(row, col));
			}
		}
	}

	return std::vector<std::pair<int, int>>();
}

std::pair<int, int> PlayerSmart::selectAttackPositionFromEmptyPositions(std::vector<std::pair<int, int>>& emptyPositions)
{
	return *GameUtils::randomElement(emptyPositions.begin(), emptyPositions.end());
}

IBattleshipGameAlgo* GetAlgorithm()
{
	_instancesVec.push_back(new PlayerSmart());			// Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1];		// Return last instance
}
