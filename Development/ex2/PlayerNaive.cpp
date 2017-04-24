#include "PlayerNaive.h"


bool PlayerNaive::init(const std::string& path)
{
	std::vector<std::pair<int, int>> moves;
	for (int row = 1; row <= BOARD_ROWS; row++) {
		for (int col = 1; col <= BOARD_COLS; col++) {
			moves.push_back(std::pair<int, int>(row, col));
		}
	}
	
	_possibleMoves = moves;
	_possibleMovesIterator = _possibleMoves.begin();

	return true;
}

std::pair<int, int> PlayerNaive::attack()
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("attack() was called before setBoard was called!");
	}

	if (_possibleMovesIterator == _possibleMoves.end())
	{
		return ATTACK_END;
	}
	std::pair<int, int> retCoord = *_possibleMovesIterator++;
	const GameBoard& opponentBoard = _opponentBoard;
	while (opponentBoard(retCoord.first, retCoord.second) != EMPTY && _possibleMovesIterator != _possibleMoves.end()) {
		retCoord = *_possibleMovesIterator++;
	}
	if (_possibleMovesIterator == _possibleMoves.end())
	{
		return ATTACK_END;
	}
	return retCoord;
}

IBattleshipGameAlgo* GetAlgorithm()
{
	_instancesVec.push_back(new PlayerNaive());			// Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1];		// Return last instance
}