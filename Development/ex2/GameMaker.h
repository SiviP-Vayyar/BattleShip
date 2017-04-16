#pragma once

#include "PlayerBase.h"

class GameMaker
{
public:
	GameMaker(int argc, char* argv[]);
	GameMaker(const GameMaker& other) = delete; // We intend to instantiate only once
	~GameMaker(){};

	GameMaker& operator=(const GameMaker& otheBoard) = delete; // We intend to instantiate only once

	void RunGame();
	std::vector<std::pair<int, int>> getMovesFromFile(const std::string& movesFilePath, const GameBoard& opponentBoard) const;
	static std::stringstream& GameMaker::skipSpaces(std::stringstream& s);


private:
	PlayerBase* _playerA; //TODO: make unique
	PlayerBase* _playerB; //TODO: make unique
	GameBoard _board;
	
	std::string _inputFolder;
	std::string _boardFilePath;
	std::string _attackFilePathA;
	std::string _attackFilePathB;

	bool ParseInput(int argc, char* argv[], std::string& path);
	bool SetAndValidateBoards();
};
