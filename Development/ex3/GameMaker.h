#pragma once

#include "GameBoard.h"
#include "GameResult.h"

typedef IBattleshipGameAlgo IAlgo;

class GameMaker
{
public:
	GameMaker(IAlgo* playerA, IAlgo* playerB, const GameBoard& board) : _playerA(playerA), _playerB(playerB), _board(board) {}
	GameMaker(const GameMaker& other) = delete; // We intend to instantiate only once

	GameMaker& operator=(const GameMaker& otheBoard) = delete; // We intend to instantiate only once

	GameResult RunGame();

private:
	IAlgo* _playerA = nullptr;
	IAlgo* _playerB = nullptr;
	std::string _nameA;
	std::string _nameB;
	GameBoard _board;
};
