#pragma once

#include "ISubmarinesGameAlgo.h"

class MyGameAlgo : public ISubmarinesGameAlgo
{
	MyGameAlgo();
	~MyGameAlgo();

	void setBoard(const char** board, int numRows, int numCols) override; // called once to notify player on his board
	std::pair<int, int> attack() override; // ask player for his move
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
};