#pragma once

#include "PlayerBase.h"

class PlayerFile : public PlayerBase
{
public:
	PlayerFile() : PlayerBase() {};
	~PlayerFile() = default;

	/* When I want to make a move on opponent's board*/
	std::pair<int, int> attack() override;

	/* Called once to allow init from files if needed returns whether the init succeeded or failed*/
	bool init(const std::string& path) override;

private:
	/*Hard coded algorithm*/
	void SetMoves(std::vector<std::pair<int, int>> moves);
	std::vector<std::pair<int, int>> getMovesFromFile(const std::string& movesFilePath) const;

	std::string _algoMovesFile;
	std::vector<std::pair<int, int>> _myMoves;
	std::vector<std::pair<int, int>>::iterator _movesIterator;
};
