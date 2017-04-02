#include "Player.h"


void Player::setBoard(const char** board, int numRows, int numCols)
{
	_myBoard = GameBoard(board, numRows, numCols);
	// TODO: need to init _opponentBoard somehow. Make a GameBoard constructor with only dimentions.
}

void Player::SetMoves(std::vector<std::pair<int, int>> moves)
{
	_myMoves = moves;
	_movesIterator = moves.begin();
}

std::pair<int, int> Player::attack()
{
	if(_player == PLAYER_NOT_YET_KNOWN)
	{
		// If attack() was called first, I am player A
		_player = PLAYER_A;
	}
	
	//TODO: test for: not missing first move, giving back right move, finishing right
	/*Return next move, and advance the iterator*/
	if(_movesIterator != _myMoves.end())
	{
		return *_movesIterator++;
	}
	return ATTACK_END;
}

void Player::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if(_player == PLAYER_NOT_YET_KNOWN)
	{
		// If notifyOnAttackResult() was called first, I am player B
		_player = PLAYER_B;
	}
	/*Getting feedback for my attack (while ignoring ATTACK_END and all positions outside the board)*/
	if(player == _player &&  _opponentBoard.isInBoard(row, col))
	{
		_opponentBoard(row, col) = attackResultToChar(result);
		updateOpponentBoardAfterAttack(row, col, attackResultToChar(result));
	}
}

char Player::attackResultToChar(AttackResult result) const
{
	switch(result)
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
		return 'X'; // Just so all paths has return values
	}
}

void Player::updateOpponentBoardAfterAttack(int row, int col, char attackChar) //NOAM
{
	//TODO:
	/*
	 * After we attack, we get this feedback from game maker, we need to update the board:
	 * 1) Miss - just mark it
	 * 2) Hit - mark it, in case it is 2-3 squares streak, we can update some MISSes around it
	 * 3) Sink - mark it, replace all HITs of that ship with SINKs, mark it's perimiter with MISSes.
	 */
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
			return; 
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
	//TODO: On scaleup - implement smart board state deduction and update after miss
}

void Player::deduceOpponentBoardAfterHit(int row, int col)
{
	//TODO: On scaleup - implement smart board state deduction and update after hit
}

void Player::deduceOpponentBoardAfterSink(int row, int col)
{
	//TODO: On scaleup - implement smart board state deduction and update after sink
}

void Player::markOpponentBoardAfterSink(int row, int col)
{
	_opponentBoard(row, col) = SINK;
	auto surroundingCordinates = _opponentBoard.getAdjacentCordinatesAsVector(row, col);

	for (auto cordinates : surroundingCordinates) 
	{
		if (_opponentBoard(cordinates.first, cordinates.second) == HIT)
		{
			markOpponentBoardAfterSink(cordinates.first, cordinates.second);
		}
	}
}