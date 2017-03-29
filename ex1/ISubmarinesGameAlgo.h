#pragma once

#include <utility> // for std::pair

enum class AttackResult {
	Miss, Hit, Sink
};

class ISubmarinesGameAlgo {
public:
	virtual ~ISubmarinesGameAlgo() = default;
	virtual void setBoard(const char** board, int numRows, int numCols); // called once to notify player on his board
	virtual std::pair<int, int> attack(); // ask player for his move
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result); // notify on last move result
};
