#pragma once
#include "GameBoard.h"
#include "IBattleshipGameAlgo.h"
#include <vector>

/*Player Macros*/
#define PLAYER_NOT_YET_KNOWN	-1
#define PLAYER_A				0
#define PLAYER_B				1
#define ATTACK_END				std::pair<int, int>(INT32_MAX, INT32_MAX) // To mark the end of the moves list

/*Board Macros*/
#define HIT						'H'
#define MISS 					'O'
#define SINK 					'S'
#define MAX_SHIPS 				5

class Player : public IBattleshipGameAlgo
{
public:
	Player() : _player(PLAYER_NOT_YET_KNOWN)
	{};
	~Player() {};

	/* Will be called once from outside the class, to init the board*/
	void setBoard(const char** board, int numRows, int numCols) override;
	
	/* When I want to make a move on opponent's board*/
	std::pair<int, int> attack() override;

	/* The algorithm is telling us what was the last move of the game*/
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	/*Hard coded algorithm*/
	void SetMoves(std::vector<std::pair<int, int>> moves);

	/*check if ship belongs to this player*/
	bool isPlayerShip(char shipType) const { return (shipType != EMPTY && GameBoard::playerShipType(_player, shipType) == shipType); }

private:
	int _player; // figure out which player I am according to first attack()/notifyOnAttackResult() call
	GameBoard _myBoard;
	GameBoard _opponentBoard;
	std::vector<std::pair<int,int>> _myMoves;
	std::vector<std::pair<int, int>>::iterator _movesIterator;

	static char attackResultToChar(AttackResult result);
	void updateOpponentBoardAfterAttack(int row, int col, char attackChar);
	void updateOpponentBoardAfterMiss(int row, int col);
	void updateOpponentBoardAfterSink(int row, int col);
	void updateOpponentBoardAfterHit(int row, int col);
	void deduceOpponentBoardAfterMiss(int row, int col);
	void deduceOpponentBoardAfterHit(int row, int col);
	void deduceOpponentBoardAfterSink(int row, int col);
	void markOpponentBoardAfterSink(int row, int col);
};
