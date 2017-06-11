#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"
#include <vector>

/* A static vector to hold all instances of algo in the dll
 * Caller (Game Manager) is responsible to delete all instances!
 */
static std::vector<IBattleshipGameAlgo *> _instancesVec;

class PlayerBase : public IBattleshipGameAlgo
{
public:
	PlayerBase() : _player(PLAYER_NOT_YET_KNOWN) { };
	virtual ~PlayerBase() = default;

	void setBoard(const BoardData& board) override;
	void setPlayer(int player) override { _player = player; };

	/* The algorithm is telling us what was the last move of the game*/
	void  notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;

protected:
	int _player;
	GameBoard _myBoard;
	GameBoard _opponentBoard;

	/*Same for every player*/
	static char attackResultToChar(AttackResult result);
	void updateOpponentBoardAfterBoardInit();
	void updateOpponentBoardAfterAttack(Coordinate coord, char attackChar);
	void updateOpponentBoardAfterMiss(Coordinate coord);
	void updateOpponentBoardAfterSink(Coordinate coord);
	void updateOpponentBoardAfterHit(Coordinate coord);
	void markOpponentBoardAfterSink(Coordinate coord);
	void deduceOpponentBoardAfterHit(Coordinate coord);
	void deduceOpponentBoardAfterSink(Coordinate coord);
};
