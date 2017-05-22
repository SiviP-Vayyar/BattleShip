#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"

typedef IBattleshipGameAlgo IAlgo;

struct GameResult
{
	GameResult() : GameResult(0, 0) {}
	GameResult(int A, int B) : scoreA(A), scoreB(B) {}
	int scoreA;
	int scoreB;
	int Winner() const
	{
		if(scoreB == scoreA)
		{
			return PLAYER_NOT_YET_KNOWN;
		}
		return scoreA > scoreB ? PLAYER_A : PLAYER_B;
	}
};

class GameMaker
{
public:
	GameMaker(IAlgo* playerA, IAlgo* playerB, const GameBoard& board) : _playerA(playerA), _playerB(playerB), _board(board) {}
	GameMaker(const GameMaker& other) = delete; // We intend to instantiate only once
	~GameMaker();

	GameMaker& operator=(const GameMaker& otheBoard) = delete; // We intend to instantiate only once

	GameResult RunGame();

private:
	IAlgo* _playerA = nullptr;
	IAlgo* _playerB = nullptr;
	GameBoard _board;
};
