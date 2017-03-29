#pragma once
#include "ArgParser.h"
#include "Player.h"

class GameMaker
{
public:
	GameMaker(int argc, char* argv[]);
	~GameMaker();

	GameMaker& operator=(const GameMaker& otheBoard); //TODO: take exact syntax from presentation and implement

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
