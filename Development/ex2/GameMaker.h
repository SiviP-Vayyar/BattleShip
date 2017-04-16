#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"
#include <functional>

typedef IBattleshipGameAlgo IAlgo;
typedef IAlgo *(*GetAlgoFuncType)();
//struct LibData
//{
//	std::string name;
//	HINSTANCE handle;
//	std::function<GetAlgoFuncType> GetAlgorithm;
//};

class GameMaker
{
public:
	GameMaker(int argc, char* argv[]);
	GameMaker(const GameMaker& other) = delete; // We intend to instantiate only once
	~GameMaker(){};

	GameMaker& operator=(const GameMaker& otheBoard) = delete; // We intend to instantiate only once

	void RunGame();

private:
	IAlgo* _playerA; //TODO: make unique
	IAlgo* _playerB; //TODO: make unique
	GameBoard _board;
	
	std::string _inputFolder;
	std::string _boardFilePath;
	std::string _attackFilePathA;
	std::string _attackFilePathB;

	bool ParseInput(int argc, char* argv[]);
	bool SetAndValidateBoards();
};
