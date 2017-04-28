#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"
#include <functional>
#include <windows.h>

typedef IBattleshipGameAlgo IAlgo;
typedef IAlgo*(*GetAlgoFuncType)();

#define GET_ALGORITHM_STR "GetAlgorithm"

struct AlgoData
{
	std::string algoFile;
	HINSTANCE handle;
};

class GameMaker
{
public:
	GameMaker(int argc, char* argv[]);
	GameMaker(const GameMaker& other) = delete; // We intend to instantiate only once
	~GameMaker();

	GameMaker& operator=(const GameMaker& otheBoard) = delete; // We intend to instantiate only once

	bool isInitSuccess() { return _initSuccess; }
	void RunGame();

private:
	IAlgo* _playerA = nullptr;
	IAlgo* _playerB = nullptr;
	GameBoard _board;
	bool _initSuccess;

	std::string _inputFolder;
	std::string _boardFilePath;
	AlgoData _algoDataA;
	AlgoData _algoDataB;

	bool ParseInput(int argc, char* argv[]);
	bool SetAndValidateBoardsAndAlgos();
	bool LoadAndInitAlgos();
};
