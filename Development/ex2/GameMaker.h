#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"
#include <functional>
#include <memory>

typedef IBattleshipGameAlgo IAlgo;
typedef IAlgo *(*GetAlgoFuncType)();

#define GET_ALGORITHM_STR "GetAlgorithm"

class GameMaker
{
public:
	GameMaker(int argc, char* argv[]);
	GameMaker(const GameMaker& other) = delete; // We intend to instantiate only once
	~GameMaker(){};

	GameMaker& operator=(const GameMaker& otheBoard) = delete; // We intend to instantiate only once

	void RunGame();

private:
	std::shared_ptr<IAlgo> _playerA;
	std::shared_ptr<IAlgo> _playerB;
	GameBoard _board;
	
	std::string _inputFolder = nullptr;
	std::string _boardFilePath = nullptr;
	std::string _algoFileA = nullptr;
	std::string _algoFileB = nullptr;

	bool ParseInput(int argc, char* argv[]);
	bool SetAndValidateBoardsAndAlgos();
	bool LoadAndInitAlgos();
};
