#include "PlayerSmart.h"


/* The algorithm is telling us what was the last move of the game*/
void  PlayerSmart::notifyOnAttackResult(int player, Coordinate coord, AttackResult result)
{
	PlayerBase::notifyOnAttackResult(player, coord, result);
	bool myShip = GameBoard::isPlayerShip(_player, _myBoard(coord)); // If not self attack - Assuming one square can be occupied by one player only
	if (!myShip) // The common case
	{
		switch (result)
		{
		case AttackResult::Hit:
		{
			_lastHitCoords.push_back(coord);
			_state = AttackState::Found;
			break;
		}
		case AttackResult::Miss:
		{
			break;
		}
		case AttackResult::Sink:
		{
			_state = AttackState::Seeking;
			int shipSize = _lastHitCoords.size() + 1;
			_lastHitCoords.clear();
			auto elemToRemove = std::find(_shipsLengthsVector.begin(), _shipsLengthsVector.end(), shipSize);
			_shipsLengthsVector.erase(elemToRemove);
			break;
		}
		default:
		{
			break; // Just so all paths has return values
		}
		}
	}
}


void PlayerSmart::setBoard(const BoardData& board) {
	PlayerBase::setBoard(board);
	_lastHitCoords.empty();
	_state = AttackState::Seeking;
	_shipsLengthsVector = _myBoard.getShipsOnBoardSizes();
}


Coordinate PlayerSmart::attack()
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("attack() was called before setBoard was called!");
	}
	Coordinate retCoord = ATTACK_END;
	switch (_state)
	{
	case AttackState::Seeking:
	{
		HeatMap heatmap = HeatMap(_myBoard, _opponentBoard, _shipsLengthsVector);
		retCoord = heatmap.hottestCoordinate();
		break;
	}
	case AttackState::Found:
	{
		retCoord = getNextCoordAfterHit();
		break;
	}
	}
	return retCoord;
}


Coordinate PlayerSmart::getNextCoordAfterHit()
{
	std::vector<Coordinate> possibleCoords = std::vector<Coordinate>();

	for (auto coord : _lastHitCoords)
	{
		auto adjCoords = _opponentBoard.getAdjacentCoordinatesAsVector(coord.row, coord.col, coord.depth);
		for (auto adjCoord : adjCoords)
		{
			if (_opponentBoard(adjCoord) == EMPTY)
			{
				possibleCoords.push_back(adjCoord);
			}
		}
	}

	HeatMap heatmap = HeatMap(_myBoard, _opponentBoard, _shipsLengthsVector);
	Coordinate retCoord = ATTACK_END;
	int maxHeat = 0;
	for (auto possibleCoord : possibleCoords)
	{
		int coordHeat = heatmap.countPossibleShipsForCoordinate(possibleCoord);
		if (coordHeat > maxHeat)
		{
			maxHeat = coordHeat;
			retCoord = possibleCoord;
		}
	}
	return retCoord;
}


std::vector<Coordinate> PlayerSmart::getAllPositions(char type) const
{
	std::vector<Coordinate> positions;
	const GameBoard& opponentBoard = _opponentBoard;

	for (int row = 1; row <= opponentBoard.rows(); row++)
	{
		for (int col = 1; col <= opponentBoard.cols(); col++)
		{
			for (int depth = 0; depth <= opponentBoard.depth(); depth++)
			{
				if (opponentBoard(row, col, depth) == type)
				{
					positions.push_back(Coordinate(row, col, depth));
				}
			}
		}
	}

	return positions;
}

Coordinate PlayerSmart::selectAttackPositionFromEmptyPositions(std::vector<Coordinate>& emptyPositions)
{
	return *GameUtils::randomElement(emptyPositions.begin(), emptyPositions.end());
}

IBattleshipGameAlgo* GetAlgorithm()
{
	_instancesVec.push_back(new PlayerSmart()); // Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1]; // Return last instance
}
