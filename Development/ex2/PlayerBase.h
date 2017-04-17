#pragma once

#include "GameBoard.h"
#include <vector>


static std::vector<IBattleshipGameAlgo *> _instancesVec; // A static vector to hold all instances of algo in the dll

class PlayerBase : public IBattleshipGameAlgo
{
public:
	PlayerBase() : _player(PLAYER_NOT_YET_KNOWN)
	{};
	~PlayerBase();

	/* Will be called once from outside the class, to init the board*/
	void setBoard(int player, const char** board, int numRows, int numCols) override;

	/* The algorithm is telling us what was the last move of the game*/
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	/* check if ship belongs to this player*/
	bool isPlayerShip(char shipType) const { return (shipType != EMPTY && GameBoard::playerShipType(_player, shipType) == shipType); }

protected:
	int _player;
	GameBoard _myBoard;
	GameBoard _opponentBoard;

	/*Same for every player*/
	static char attackResultToChar(AttackResult result);
	void updateOpponentBoardAfterAttack(int row, int col, char attackChar);
	void updateOpponentBoardAfterMiss(int row, int col);
	void updateOpponentBoardAfterSink(int row, int col);
	void updateOpponentBoardAfterHit(int row, int col);
	void markOpponentBoardAfterSink(int row, int col);
	void deduceOpponentBoardAfterMiss(int row, int col);
	void deduceOpponentBoardAfterHit(int row, int col);
	void deduceOpponentBoardAfterSink(int row, int col);
};