#pragma once
#include "ArgParser.h"
#include "Player.h"

class GameMaker
{
public:
	GameMaker(int argc, char* argv[]);
	~GameMaker();
	//TODO: copy constructor

	GameMaker& operator=(const GameMaker& otheBoard) = delete; // We intend to instantiate only once

	void RunGame();


private:
	ArgParser _parser;
	Player _playerA;
	Player _playerB;
	GameBoard _boardA;
	GameBoard _boardB;

	void SetConfiguration();
	bool ValidateInput(const std::string& path);
	bool ValidateBoards();
};
