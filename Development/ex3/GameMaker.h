#pragma once

#include "GameBoard.h"
#include "GameResult.h"
#include <memory>

typedef IBattleshipGameAlgo IAlgo;

class GameMaker
{
public:
	GameMaker(std::shared_ptr<IAlgo> playerA, std::shared_ptr<IAlgo> playerB, const GameBoard& board, std::string nameA_, std::string nameB_)
	: _playerA(playerA)
	, _playerB(playerB)
	, _board(board)
	, _nameA(nameA_)
	, _nameB(nameB_) {}

	GameMaker(const GameMaker& other) = delete; // We intend to instantiate only once
	GameMaker& operator=(const GameMaker& otheBoard) = delete; // We intend to instantiate only once

	GameResult RunGame();

	bool AttackSafe(Coordinate& attackPosition, IAlgo* currentPlayer, double* currentPlayerTime);
	bool NotifySafe(IAlgo* currentPlayer, int& currentPlayerDef, Coordinate& attackPosition, AttackResult& attackResult, GameResult& result);

private:
	std::shared_ptr<IAlgo> _playerA = nullptr;
	std::shared_ptr<IAlgo> _playerB = nullptr;
	std::string _nameA;
	std::string _nameB;
	GameBoard _board;
};
