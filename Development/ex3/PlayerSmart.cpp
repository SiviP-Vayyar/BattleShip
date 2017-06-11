#include "PlayerSmart.h"
#include "GameUtils.h"
#include "HeatMap.h"


/* The algorithm is telling us what was the last move of the game*/
void  PlayerSmart::notifyOnAttackResult(int player, Coordinate coord, AttackResult result)
{
	PlayerBase::notifyOnAttackResult(player, coord, result);
	bool myShip = GameBoard::isPlayerShip(_player, _myBoard(coord)); // If not self attack - Assuming one square can be occupied by one player only
	if (!myShip)
	{
		std::unordered_set<Coordinate> coords;
		switch(result)
		{
		case(AttackResult::Hit):
			_allHitCoords.insert(coord);
			break;
					
		case(AttackResult::Sink):
			_opponentBoard.getShipCoordinates(coord.row, coord.col, coord.depth, coords);
			_shipsLengthsVector.erase(std::find(_shipsLengthsVector.begin(), _shipsLengthsVector.end(), coords.size()));
			_allHitCoords -= coords;
			break;
		default: 
			break;
		}
	}
}


void PlayerSmart::setBoard(const BoardData& board) {
	PlayerBase::setBoard(board);
	_shipsLengthsVector = _myBoard.getShipsOnBoardSizes();
}


Coordinate PlayerSmart::attack()
{
	if(_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("attack() was called before setBoard was called!");
	}

	// first search for hit on the board and try to expand it
	if (!_allHitCoords.empty())
	{
		return getNextCoordAfterHit();
	}

	//else search entire board
	HeatMap heatmap(_opponentBoard, _shipsLengthsVector);
	return heatmap.hottestCoordinate();
}


Coordinate PlayerSmart::getNextCoordAfterHit()
{
	std::vector<Coordinate> possibleCoords;

	for (auto& coord : _allHitCoords)
	{
		auto adjCoords = _opponentBoard.getAdjacentCoordinatesAsVector(coord.row, coord.col, coord.depth);
		for (auto& adjCoord : adjCoords)
		{
			if (_opponentBoard(adjCoord) == EMPTY)
			{
				possibleCoords.push_back(adjCoord);
			}
		}
	}

	HeatMap heatmap(_opponentBoard, _shipsLengthsVector);
	Coordinate retCoord = ATTACK_END;
	int maxHeat = 0;
	for (auto& possibleCoord : possibleCoords)
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


std::unordered_set<Coordinate> PlayerSmart::getAllPositions(char type) const
{
	std::unordered_set<Coordinate> positions;
	const GameBoard& opponentBoard = _opponentBoard;

	for (int row = 1; row <= opponentBoard.rows(); row++)
	{
		for (int col = 1; col <= opponentBoard.cols(); col++)
		{
			for (int depth = 0; depth <= opponentBoard.depth(); depth++)
			{
				if (opponentBoard(row, col, depth) == type)
				{
					positions.insert(Coordinate(row, col, depth));
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
