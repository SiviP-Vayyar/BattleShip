#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"
#include "PlayerBase.h"
#include <vector>
#include <algorithm>  

class PlayerSmart : public PlayerBase
{
public:
	PlayerSmart() : PlayerBase() {}
	~PlayerSmart() = default;

	/* Will be called once from outside the class, to init the board*/
	void setBoard(const BoardData& board) override;

	Coordinate attack() override;

	/* The algorithm is telling us what was the last move of the game*/
	void  notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;


protected:
	std::vector<int> _shipsLengthsVector;
	std::unordered_set<Coordinate> _allHitCoords;

	Coordinate getNextCoordAfterHit();
	std::unordered_set<Coordinate> getAllPositions(char type) const;
	static Coordinate selectAttackPositionFromEmptyPositions(std::vector<Coordinate>& emptyPositions);
};

