#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"
#include <vector>

static std::vector<IBattleshipGameAlgo *> _instancesVec; // A static vector to hold all instances of algo in the dll

class PlayerBase : public IBattleshipGameAlgo
{
public:
	PlayerBase() : _player(PLAYER_NOT_YET_KNOWN) { };
	virtual ~PlayerBase();

	/* Will be called once from outside the class, to init the board*/
	void setBoard(int player, const char** board, int numRows, int numCols) override;

	/* The algorithm is telling us what was the last move of the game*/
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

protected:
	int _player;
	GameBoard _myBoard;
	GameBoard _opponentBoard;

	/*Same for every player*/
	static char attackResultToChar(AttackResult result);
	void updateOpponentBoardAfterBoardInit();
	void updateOpponentBoardAfterAttack(int row, int col, char attackChar);
	void updateOpponentBoardAfterMiss(int row, int col);
	void updateOpponentBoardAfterSink(int row, int col);
	void updateOpponentBoardAfterHit(int row, int col);
	void markOpponentBoardAfterSink(int row, int col);
	void deduceOpponentBoardAfterHit(int row, int col);
	void deduceOpponentBoardAfterSink(int row, int col);
};
