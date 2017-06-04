#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"
#include "PlayerBase.h"
#include "GameUtils.h"
#include "HeatMap.h"
#include <vector>
#include <algorithm>  


enum class AttackState
{
	Seeking,
	Found
};

class PlayerSmart : public PlayerBase
{
public:
	PlayerSmart() : PlayerBase(), _state(AttackState::Seeking), _lastHitCoords(std::vector<Coordinate>()){};
	~PlayerSmart() = default;

	/* Will be called once from outside the class, to init the board*/
	void setBoard(const BoardData& board) override;

	Coordinate attack() override;

	/* The algorithm is telling us what was the last move of the game*/
	void  notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;


protected:
	std::vector<Coordinate> _lastHitCoords;
	std::vector<int> _shipsLengthsVector;
	AttackState _state;

	Coordinate getNextCoordAfterHit();
	std::vector<Coordinate> getAllPositions(char type) const;
	static Coordinate selectAttackPositionFromEmptyPositions(std::vector<Coordinate>& emptyPositions);
};

