#include "PlayerNaive.h"


std::pair<int, int> PlayerNaive::attack()
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("attack() was called before setBoard was called!");
	}

	//TODO: implement
	return std::make_pair(0, 0);
}

IBattleshipGameAlgo* GetAlgorithm()
{
	_instancesVec.push_back(new PlayerNaive());			// Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1];		// Return last instance
}