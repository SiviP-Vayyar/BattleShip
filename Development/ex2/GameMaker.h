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
	
	std::string _inputFolder;
	std::string _boardFilePath;
	std::string _algoFileA;
	std::string _algoFileB;

	bool ParseInput(int argc, char* argv[]);
	bool SetAndValidateBoardsAndAlgos();
	bool LoadAndInitAlgos();
};
