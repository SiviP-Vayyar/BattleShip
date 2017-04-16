#pragma once

#include "PlayerBase.h"

class PlayerFile : public PlayerBase
{
public:
	PlayerFile() : PlayerBase()
	{};
	~PlayerFile() {};

	/* When I want to make a move on opponent's board*/
	std::pair<int, int> attack() override;
	
	/* Called once to allow init from files if needed returns whether the init succeeded or failed*/
	bool init(const std::string& path) override;

private:
	/*Hard coded algorithm*/
	void SetMoves(std::vector<std::pair<int, int>> moves);
	std::vector<std::pair<int, int>> getMovesFromFile(const std::string& movesFilePath) const;
	static std::stringstream& skipSpaces(std::stringstream& s);

	std::string _algoMovesFolder;
	std::vector<std::pair<int,int>> _myMoves;
	std::vector<std::pair<int, int>>::iterator _movesIterator;
};
