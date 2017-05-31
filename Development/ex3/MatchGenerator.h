﻿#pragma once

#include "AlgoData.h"
#include "GameResult.h"
#include <mutex>
#include "HouseEntry.h"
#include <map>

class MatchGenerator
{
public:
	explicit MatchGenerator(const std::vector<AlgoData>& house);
	std::pair<std::vector<AlgoData>::const_iterator, std::vector<AlgoData>::const_iterator> GetNextMatch();
	boolean IsValidMatch(const std::vector<AlgoData>::const_iterator& playerAiter, const std::vector<AlgoData>::const_iterator& playerBiter) const;
	void ResetIterators() { _playerAiter = _players.cbegin(); _playerBiter = _players.cbegin(); }
	void updateHouseEntry(const std::string& name, const GameResult& result, int player);

private:
	friend class TournamentMaker;
	const std::vector<AlgoData>& _players;
	std::vector<AlgoData>::const_iterator _playerAiter;
	std::vector<AlgoData>::const_iterator _playerBiter;
	std::map<std::string, HouseEntry> _houseEntries;
	std::mutex matches_lock, score_lock;
};