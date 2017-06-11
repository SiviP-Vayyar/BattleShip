#pragma once

#include "AlgoData.h"
#include "GameResult.h"
#include <mutex>
#include "HouseEntry.h"
#include <map>

class MatchGenerator
{
public:
	explicit MatchGenerator(const std::vector<std::shared_ptr<AlgoData>>& house);
	std::pair<std::vector<std::shared_ptr<AlgoData>>::const_iterator, std::vector<std::shared_ptr<AlgoData>>::const_iterator> GetNextMatch();
	boolean IsValidMatch(const std::vector<std::shared_ptr<AlgoData>>::const_iterator& playerAiter, const std::vector<std::shared_ptr<AlgoData>>::const_iterator& playerBiter) const;
	void ResetIterators() { _playerAiter = _players.cbegin(); _playerBiter = _players.cbegin(); }
	void updateHouseEntry(const std::string& name, const GameResult& result, int player);

private:
	friend class TournamentMaker;
	const std::vector<std::shared_ptr<AlgoData>>& _players;
	std::vector<std::shared_ptr<AlgoData>>::const_iterator _playerAiter;
	std::vector<std::shared_ptr<AlgoData>>::const_iterator _playerBiter;
	std::map<std::string, HouseEntry> _houseEntries;
	std::mutex matches_lock, score_lock, board_lock;

	// GetNextBoard members
	size_t _currBoardIdx;
};
