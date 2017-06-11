#pragma once

#include "AlgoData.h"
#include "GameResult.h"
#include <mutex>
#include "HouseEntry.h"
#include <map>

class MatchGenerator
{
public:
	explicit MatchGenerator(const std::vector<std::shared_ptr<AlgoData>>& house, bool playModePersistentPlayers);
	std::pair<std::shared_ptr<const AlgoData>, std::shared_ptr<const AlgoData>> GetNextMatch();
	static bool IsValidMatch(std::shared_ptr<const AlgoData>> playerAptr, std::shared_ptr<const AlgoData>> playerBptr);
	void ResetIterators();
	void updateHouseEntry(const std::string& name, const GameResult& result, int player);

private:
	friend class TournamentMaker;
	const std::vector<std::shared_ptr<AlgoData>>& _players;
	bool _gameModeReuse;
	std::vector<std::pair<std::shared_ptr<const AlgoData>, std::shared_ptr<const AlgoData>>> _possibleMatches;
	std::map<std::string, HouseEntry> _houseEntries;
	std::mutex matches_lock, score_lock, board_lock;

	// GetNextBoard members
	size_t _currBoardIdx;
};
