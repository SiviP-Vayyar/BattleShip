#include "PlayerBase.h"
#include "GameUtils.h"

void PlayerBase::setBoard(const BoardData& board)
{
	_myBoard = GameBoard(board); // init my board from given board
	cube opponentBoard = GameBoard::newEmptyRawBoard(board.rows(), board.cols(), board.depth()); // local empty board allocation
	_opponentBoard = GameBoard(opponentBoard, board.rows(), board.cols(), board.depth()); // set empty opponent board
	updateOpponentBoardAfterBoardInit();//mark all places that can't contain opponent ships
}

void PlayerBase::notifyOnAttackResult(int player, Coordinate coord, AttackResult result)
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("Notified on attack before setBoard was called!");
	}

	/*Getting feedback for an attack (while ignoring ATTACK_END and all positions outside the board)*/
	if (_opponentBoard.isInBoard(coord))
	{
		bool myShip = GameBoard::isPlayerShip(_player, _myBoard(coord)); // If not self attack - Assuming one square can be occupied by one player only
		if (!myShip) // The common case
		{
			updateOpponentBoardAfterAttack(coord, attackResultToChar(result));
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
			for (int depth = 1; depth <= _myBoard.depth(); depth++)
			{
				if (GameBoard::isPlayerShip(_player, _myBoard(row, col, depth)))
				{//check if the coordinates are of this player's ship
					updateOpponentBoardAfterMiss(Coordinate(row, col, depth));
					for (auto adjCoord : _myBoard.getAdjacentCoordinatesAsVector(row, col, depth))
					{
						updateOpponentBoardAfterMiss(adjCoord);
					}
				}
			}
		}
	}
}

void PlayerBase::updateOpponentBoardAfterAttack(Coordinate coord, char attackChar)
{
	switch (attackChar)
	{
	case MISS:
	{
		updateOpponentBoardAfterMiss(coord);
		return;
	}
	case HIT:
	{
		updateOpponentBoardAfterHit(coord);
		return;
	}
	case SINK:
	{
		updateOpponentBoardAfterSink(coord);
		return;
	}
	default:
	{
		return;
	}
	}
}

void PlayerBase::updateOpponentBoardAfterMiss(Coordinate coord)
{
	if (_opponentBoard(coord) == EMPTY)
	{
		_opponentBoard(coord) = MISS;
	}
}

void PlayerBase::updateOpponentBoardAfterHit(Coordinate coord)
{
	if (_opponentBoard(coord) == EMPTY)
	{
		_opponentBoard(coord) = HIT;
		deduceOpponentBoardAfterHit(coord);
	}
}

void PlayerBase::updateOpponentBoardAfterSink(Coordinate coord)
{
	markOpponentBoardAfterSink(coord);
	deduceOpponentBoardAfterSink(coord);
}

void PlayerBase::markOpponentBoardAfterSink(Coordinate coord)
{
	_opponentBoard(coord) = SINK;
	auto surroundingCoordinates = _opponentBoard.getAdjacentCoordinatesAsVector(coord.row, coord.col, coord.depth);

	for (auto Coordinates : surroundingCoordinates)
	{
		if (_opponentBoard(Coordinates) == HIT)
		{
			markOpponentBoardAfterSink(Coordinates);
		}
	}
}

void PlayerBase::deduceOpponentBoardAfterHit(Coordinate coord)
{
	auto adjCoordinates = _opponentBoard.getAdjacentCoordinatesAsVector(coord.row, coord.col, coord.depth);
	auto diagCoordinates = _opponentBoard.getDiagonalCoordinatesAsVector(coord);
	std::vector<Coordinate> possibleCoordinatesToMark;

	for (auto adjCoord : adjCoordinates)
	{
		if (_opponentBoard(adjCoord) == HIT)
		{
			if (adjCoord.row == coord.row && adjCoord.col == coord.col)
			{
				possibleCoordinatesToMark.push_back(Coordinate(coord.row + 1, coord.col, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row - 1, coord.col, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col + 1, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col - 1, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row + 1, coord.col, adjCoord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row - 1, coord.col, adjCoord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col + 1, adjCoord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col - 1, adjCoord.depth));
			}
			else if (adjCoord.row == coord.row && adjCoord.depth == coord.depth)
			{
				possibleCoordinatesToMark.push_back(Coordinate(coord.row + 1, coord.col, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row - 1, coord.col, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col, coord.depth + 1));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col, coord.depth - 1));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row + 1, adjCoord.col, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row - 1, adjCoord.col, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, adjCoord.col, coord.depth + 1));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, adjCoord.col, coord.depth - 1));
			}
			else if (adjCoord.col == coord.col && adjCoord.depth == coord.depth)
			{
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col + 1, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col - 1, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col, coord.depth + 1));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col, coord.depth - 1));
				possibleCoordinatesToMark.push_back(Coordinate(adjCoord.row, coord.col + 1, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(adjCoord.row, coord.col - 1, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(adjCoord.row, coord.col, coord.depth + 1));
				possibleCoordinatesToMark.push_back(Coordinate(adjCoord.row, coord.col, coord.depth - 1));
			}
		}
	}


	//TODO: we can deduce even farther if we look at two diagonal coordinates at a time
	for (auto diagCoord : diagCoordinates)
	{
		if (_opponentBoard(diagCoord) == HIT)
		{
			if (diagCoord.row == coord.row)
			{
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col, diagCoord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, diagCoord.col, coord.depth));
			}
			if (diagCoord.col == coord.col)
			{
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, coord.col, diagCoord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(diagCoord.row, coord.col, coord.depth));
			}
			if (diagCoord.depth == coord.depth)
			{
				possibleCoordinatesToMark.push_back(Coordinate(coord.row, diagCoord.col, coord.depth));
				possibleCoordinatesToMark.push_back(Coordinate(diagCoord.row, coord.col, coord.depth));
			}

		}
	}

	for (auto possibleCoord : possibleCoordinatesToMark)
	{
		if (_opponentBoard.isInBoard(possibleCoord)
			&& _opponentBoard(possibleCoord) == EMPTY)
		{
			_opponentBoard(possibleCoord) = MISS;
		}
	}
}

void PlayerBase::deduceOpponentBoardAfterSink(Coordinate coord)
{
	std::unordered_set<Coordinate> sunkShipCoordinates;
	_opponentBoard.getShipCoordinates(coord.row, coord.col, coord.depth, sunkShipCoordinates);

	for (auto sunkCoords : sunkShipCoordinates)
	{
		auto surroundingCoordinates = _opponentBoard.getAdjacentCoordinatesAsVector(sunkCoords.row, sunkCoords.col, sunkCoords.depth);
		for (auto Coordinates : surroundingCoordinates)
		{
			if (_opponentBoard(Coordinates) == EMPTY)
			{
				_opponentBoard(Coordinates) = MISS;
			}
		}
	}
}

