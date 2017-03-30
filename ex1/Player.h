#pragma once
#include "GameBoard.h"
#include "ISubmarinesGameAlgo.h"
#include <vector>

#define PLAYER_A 0
#define PLAYER_B 1

class Player : public ISubmarinesGameAlgo
{
public:
	Player();
	~Player();

	/* Will be called once from outside the class, to init the board*/
	void setBoard(const char** board, int numRows, int numCols) override;
	
	/* When I want to make a move on opponent's board*/
	std::pair<int, int> attack() override;
	
	/* The algorithm is telling us what was the last move of the game*/
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	/*Hard coded algorithm*/
	void SetMoves(const std::vector<std::pair<int, int>>& moves) { _myMoves = moves; }


private:
	int _player; // figure out which player I am according to first attack()/notifyOnAttackResult() call
	GameBoard _myBoard;
	GameBoard _opponentBoard;
	std::vector<std::pair<int,int>> _myMoves;
};
