#pragma once

#include "Player.h"

class GameMaker
{
public:
	std::vector<std::pair<int, int>> getMovesFromFile(const std::string& movesFilePath);
	GameMaker(int argc, char* argv[]);
	~GameMaker();
	//TODO: copy constructor

	GameMaker& operator=(const GameMaker& otheBoard) = delete; // We intend to instantiate only once

	void RunGame();


private:
	Player _playerA;
	Player _playerB;
	GameBoard _boardA;
	GameBoard _boardB;

	std::string _inputFolder;
	std::string _boardFilePath;
	std::string _attackFilePathA;
	std::string _attackFilePathB;

	bool ParseInput(int argc, char* argv[], std::string& path);
	bool ValidateBoards();
};
