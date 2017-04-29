#include "PlayerBase.h"
#include "GameUtils.h"

void PlayerBase::setBoard(int player, const char** board, int numRows, int numCols)
{
	_player = player;
	_myBoard = GameBoard(board, numRows, numCols); // init my board from given board
	char** opponentBoard = GameBoard::newEmptyRawBoard(numRows, numCols); // local empty board allocation
	_opponentBoard = GameBoard(opponentBoard, numRows, numCols); // set empty opponent board
	GameBoard::deleteRawBoard(opponentBoard, numRows, numCols); // delete local allocation
	updateOpponentBoardAfterBoardInit();//mark all places that can't contain opponent ships
}

void PlayerBase::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("Notified on attack before setBoard was called!");
	}

	/*Getting feedback for an attack (while ignoring ATTACK_END and all positions outside the board)*/
	if (_opponentBoard.isInBoard(row, col))
	{
		bool myShip = GameBoard::isPlayerShip(_player, _myBoard(row, col)); // If not self attack - Assuming one square can be occupied by one player only
		if (!myShip) // The common case
		{
			updateOpponentBoardAfterAttack(row, col, attackResultToChar(result));
		}
	}
}

char PlayerBase::attackResultToChar(AttackResult result)
{
	switch (result)
	{
	case AttackResult::Hit:
		{
			return HIT;
		}
	case AttackResult::Miss:
		{
			return MISS;
		}
	case AttackResult::Sink:
		{
			return SINK;
		}
	default:
		{
			return 'X'; // Just so all paths has return values
		}
	}
}

void PlayerBase::updateOpponentBoardAfterBoardInit()
{
	for (int row = 1; row <= _myBoard.rows(); row++)
	{
		for (int col = 1; col <= _myBoard.cols(); col++)
		{
			if (GameBoard::isPlayerShip(_player, _myBoard(row, col)))
			{//check if the coordinates are of this player's ship
				updateOpponentBoardAfterMiss(row, col);
				for (auto adjCoord : _myBoard.getAdjacentCoordinatesAsVector(row, col))
				{
					updateOpponentBoardAfterMiss(adjCoord.first, adjCoord.second);
				}
			}
		}
	}
}

void PlayerBase::updateOpponentBoardAfterAttack(int row, int col, char attackChar)
{
	switch (attackChar)
	{
	case MISS:
		{
			updateOpponentBoardAfterMiss(row, col);
			return;
		}
	case HIT:
		{
			updateOpponentBoardAfterHit(row, col);
			return;
		}
	case SINK:
		{
			updateOpponentBoardAfterSink(row, col);
			return;
		}
	default:
		{
			return;
		}
	}
}

void PlayerBase::updateOpponentBoardAfterMiss(int row, int col)
{
	if (_opponentBoard(row, col) == EMPTY)
	{
		_opponentBoard(row, col) = MISS;
	}
}

void PlayerBase::updateOpponentBoardAfterHit(int row, int col)
{
	if (_opponentBoard(row, col) == EMPTY)
	{
		_opponentBoard(row, col) = HIT;
		deduceOpponentBoardAfterHit(row, col);
	}
}

void PlayerBase::updateOpponentBoardAfterSink(int row, int col)
{
	markOpponentBoardAfterSink(row, col);
	deduceOpponentBoardAfterSink(row, col);
}

void PlayerBase::markOpponentBoardAfterSink(int row, int col)
{
	_opponentBoard(row, col) = SINK;
	auto surroundingCoordinates = _opponentBoard.getAdjacentCoordinatesAsVector(row, col);

	for (auto Coordinates : surroundingCoordinates)
	{
		if (_opponentBoard(Coordinates.first, Coordinates.second) == HIT)
		{
			markOpponentBoardAfterSink(Coordinates.first, Coordinates.second);
		}
	}
}

void PlayerBase::deduceOpponentBoardAfterHit(int row, int col)
{
	auto adjCoordinates = _opponentBoard.getAdjacentCoordinatesAsVector(row, col);
	auto diagCoordinates = _opponentBoard.getDiagonalCoordinatesAsVector(row, col);
	std::vector<std::pair<int, int>> possibleCoordinatesToMark;

	for (auto adjCoord : adjCoordinates)
	{
		if (_opponentBoard(adjCoord.first, adjCoord.second) == HIT)
		{
			if (adjCoord.first == row)
			{
				possibleCoordinatesToMark.push_back(std::pair<int, int>(row + 1, col));
				possibleCoordinatesToMark.push_back(std::pair<int, int>(row - 1, col));
				possibleCoordinatesToMark.push_back(std::pair<int, int>(row + 1, adjCoord.second));
				possibleCoordinatesToMark.push_back(std::pair<int, int>(row - 1, adjCoord.second));
			}
			else
			{//if ( adjCoord.second == col)
				possibleCoordinatesToMark.push_back(std::pair<int, int>(row, col + 1));
				possibleCoordinatesToMark.push_back(std::pair<int, int>(row, col - 1));
				possibleCoordinatesToMark.push_back(std::pair<int, int>(adjCoord.first, col + 1));
				possibleCoordinatesToMark.push_back(std::pair<int, int>(adjCoord.first, col - 1));
			}
		}
	}

	for (auto diagCoord : diagCoordinates)
	{
		if (_opponentBoard(diagCoord.first, diagCoord.second) == HIT)
		{
			possibleCoordinatesToMark.push_back(std::pair<int, int>(diagCoord.first, col));
			possibleCoordinatesToMark.push_back(std::pair<int, int>(row, diagCoord.second));
		}
	}

	for (auto possibleCoord : possibleCoordinatesToMark)
	{
		if (_opponentBoard.isInBoard(possibleCoord.first, possibleCoord.second)
			&& _opponentBoard(possibleCoord.first, possibleCoord.second) == EMPTY)
		{
			_opponentBoard(possibleCoord.first, possibleCoord.second) = MISS;
		}
	}
}

void PlayerBase::deduceOpponentBoardAfterSink(int row, int col)
{
	std::set<std::pair<int, int>> sunkShipCoordinates;
	_opponentBoard.getShipCoordinates(row, col, sunkShipCoordinates);

	for (auto sunkCoords : sunkShipCoordinates)
	{
		auto surroundingCoordinates = _opponentBoard.getAdjacentCoordinatesAsVector(sunkCoords.first, sunkCoords.second);
		for (auto Coordinates : surroundingCoordinates)
		{
			if (_opponentBoard(Coordinates.first, Coordinates.second) == EMPTY)
			{
				_opponentBoard(Coordinates.first, Coordinates.second) = MISS;
			}
		}
	}
}
