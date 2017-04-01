#pragma once
#include "GameBoard.h"
#include "IBattleshipGameAlgo.h"
#include <vector>

/*Player Macros*/
#define PLAYER_NOT_YET_KNOWN -1
#define PLAYER_A 0
#define PLAYER_B 1
#define ATTACK_END std::pair<int, int>(INFINITY, INFINITY) // To mark the end of the moves list

/*Board Macros*/
#define HIT 'H'
#define MISS 'O'
#define SINK 'S'
#define MAX_SHIPS 5

class Player : public IBattleshipGameAlgo
{
public:
	Player();
	~Player(){};

	/* Will be called once from outside the class, to init the board*/
	void setBoard(const char** board, int numRows, int numCols) override;
	
	/* When I want to make a move on opponent's board*/
	std::pair<int, int> attack() override;

	/* The algorithm is telling us what was the last move of the game*/
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	/*Hard coded algorithm*/
	void SetMoves(std::vector<std::pair<int, int>> moves);


private:
	int _player = PLAYER_NOT_YET_KNOWN; // figure out which player I am according to first attack()/notifyOnAttackResult() call
	GameBoard _myBoard;
	GameBoard _opponentBoard;
	std::vector<std::pair<int,int>> _myMoves;
	std::vector<std::pair<int, int>>::iterator _movesIterator;

	char attackResultToChar(AttackResult result) const;
	void updateOpponentBoardAfterAttack(int row, int col, char attackChar);
};
