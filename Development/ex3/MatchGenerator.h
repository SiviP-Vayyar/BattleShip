#pragma once

#include "AlgoData.h"
#include "GameResult.h"
#include <mutex>
#include "HouseEntry.h"
#include <map>

class MatchGenerator
{
public:
	explicit MatchGenerator(const std::vector<AlgoData>& house, bool playModePersistentPlayers);
	std::pair<const AlgoData*, const AlgoData*> GetNextMatch();
	static bool IsValidMatch(const AlgoData* playerAptr, const AlgoData* playerBptr);
	void ResetIterators();
	void updateHouseEntry(const std::string& name, const GameResult& result, int player);

private:
	friend class TournamentMaker;
	bool _gameModeReuse;
	const std::vector<AlgoData>& _players;
	std::vector<std::pair<const AlgoData*, const AlgoData*>> _possibleMatches;
	std::map<std::string, HouseEntry> _houseEntries;
	std::mutex matches_lock, score_lock, board_lock;

	// GetNextBoard members
	size_t _currBoardIdx;
};
