#include "Player.h"


void Player::setBoard(const char** board, int numRows, int numCols)
{
	
	_myBoard = GameBoard(board, numRows, numCols); // init my board from given board
	char** opponentBoard = GameBoard::newEmptyRawBoard(numRows, numCols); // local empty board allocation
	_opponentBoard = GameBoard(opponentBoard, numRows, numCols); // set empty opponent board
	GameBoard::deleteRawBoard(opponentBoard, numRows, numCols); // delete local allocation
}

void Player::SetMoves(std::vector<std::pair<int, int>> moves)
{
	_myMoves = moves;
	_movesIterator = _myMoves.begin();
}

std::pair<int, int> Player::attack()
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		/* If attack() was called first, I am player A*/
		_player = PLAYER_A;
	}

	/*Return next move, and advance the iterator*/
	if (_movesIterator != _myMoves.end())
	{
		return *_movesIterator++;
	}
	return ATTACK_END;
}

void Player::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		/* If notifyOnAttackResult() was called first, I am player B*/
		_player = PLAYER_B;
	}

	/*Getting feedback for an attack (while ignoring ATTACK_END and all positions outside the board)*/
	if (_opponentBoard.isInBoard(row, col))
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

char Player::attackResultToChar(AttackResult result)
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

void Player::updateOpponentBoardAfterAttack(int row, int col, char attackChar)
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

void Player::updateOpponentBoardAfterMiss(int row, int col)
{
	_opponentBoard(row, col) = MISS;
}

void Player::updateOpponentBoardAfterHit(int row, int col)
{
	_opponentBoard(row, col) = HIT;
	deduceOpponentBoardAfterHit(row, col);
}

void Player::updateOpponentBoardAfterSink(int row, int col) 
{
	markOpponentBoardAfterSink(row, col);
	deduceOpponentBoardAfterSink(row, col);
}

void Player::deduceOpponentBoardAfterMiss(int row, int col)
{
	//On scaleup - implement smart board state deduction and update after miss
}

void Player::deduceOpponentBoardAfterHit(int row, int col)
{
	//On scaleup - implement smart board state deduction and update after hit
}

void Player::deduceOpponentBoardAfterSink(int row, int col)
{
	//On scaleup - implement smart board state deduction and update after sink
}

void Player::markOpponentBoardAfterSink(int row, int col)
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