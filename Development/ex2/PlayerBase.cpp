#include "PlayerBase.h"


PlayerBase::~PlayerBase()
{
	for(auto it = _instancesVec.begin(); it != _instancesVec.end(); ++it)
	{
		delete(*it);
	}
}

void PlayerBase::setBoard(int player, const char** board, int numRows, int numCols)
{
	_player = player;
	_myBoard = GameBoard(board, numRows, numCols); // init my board from given board
	char** opponentBoard = GameBoard::newEmptyRawBoard(numRows, numCols); // local empty board allocation
	_opponentBoard = GameBoard(opponentBoard, numRows, numCols); // set empty opponent board
	GameBoard::deleteRawBoard(opponentBoard, numRows, numCols); // delete local allocation
}

void PlayerBase::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("Notified on attack before setBoard was called!");
	}

	/*Getting feedback for an attack (while ignoring ATTACK_END and all positions outside the board)*/
	if (_opponentBoard.isInBoard(row, col)) //TODO: make sure rules of shooting myself hasen't changed!
	{
		bool myAttack = player == _player;
		bool myShip = isPlayerShip(_myBoard(row, col)); // If not self attack - Assuming one square can be occupied by one player only
		if (myAttack && !myShip) // The common case
		{
			_opponentBoard(row, col) = attackResultToChar(result);
			updateOpponentBoardAfterAttack(row, col, attackResultToChar(result));
		}
		else if (!myAttack && !myShip) // if opponent attackted his own ship
		{
			_opponentBoard(row, col) = attackResultToChar(result);
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

void PlayerBase::updateOpponentBoardAfterAttack(int row, int col, char attackChar)
{
	switch (attackChar)
	{
		case MISS:
		{
			updateOpponentBoardAfterMiss(row, col);
			return;
		}
		case  HIT:
		{
			updateOpponentBoardAfterHit(row, col);
			return;
		}
		case SINK:
		{
			updateOpponentBoardAfterSink(row, col);
			return ;
		}
		default:
		{
			return;
		}
	}
}

void PlayerBase::updateOpponentBoardAfterMiss(int row, int col)
{
	_opponentBoard(row, col) = MISS;
}

void PlayerBase::updateOpponentBoardAfterHit(int row, int col)
{
	_opponentBoard(row, col) = HIT;
	deduceOpponentBoardAfterHit(row, col);
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

void PlayerBase::deduceOpponentBoardAfterMiss(int row, int col)
{
	//TODO: implelemnt
}

void PlayerBase::deduceOpponentBoardAfterHit(int row, int col)
{
	//TODO: implelemnt
}

void PlayerBase::deduceOpponentBoardAfterSink(int row, int col)
{
	//TODO: implelemnt
}
