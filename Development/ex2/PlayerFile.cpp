#include "PlayerFile.h"


void PlayerFile::SetMoves(std::vector<std::pair<int, int>> moves)
{
	_myMoves = moves;
	_movesIterator = _myMoves.begin();
}

std::pair<int, int> PlayerFile::attack()
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("attack() was called before setBoard was called!");
	}

	/*Return next move, and advance the iterator*/
	if (_movesIterator != _myMoves.end())
	{
		return *_movesIterator++;
	}
	return ATTACK_END;
}

IBattleshipGameAlgo* GetAlgorithm()
{
	_instancesVec.push_back(new PlayerFile());			// Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1];		// Return last instance
}