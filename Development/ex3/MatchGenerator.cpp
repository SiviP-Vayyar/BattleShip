#include "MatchGenerator.h"


MatchGenerator::MatchGenerator(const std::vector<AlgoData>& house, bool gameModeReuse)
: _gameModeReuse(gameModeReuse), _players(house)
{
	ResetIterators();
	for (auto& data : house)
	{
		_houseEntries[data->name] = HouseEntry(data->name);
	}
}

void MatchGenerator::ResetIterators()
{
	// claer and populate possible maches vector
	_possibleMatches.clear();
	for (auto& playerA : _players)
	{
		for (auto& playerB : _players)
		{
			if (playerA.name != playerB.name)
			{
				_possibleMatches.emplace_back(&playerA, &playerB);
			}
		}
	}
}

std::pair<const AlgoData*, const AlgoData*> MatchGenerator::GetNextMatch()
{
	std::pair<AlgoData*, AlgoData*> noMoreMatches(nullptr, nullptr);

	// when we know each player is a new instance - can have same player in multiple matche simultaneously
	if (!_gameModeReuse)
	{
		std::lock_guard<std::mutex> guard(matches_lock);

		if (_possibleMatches.empty())
		{
			return noMoreMatches;
		}

		auto firstElemItr =_possibleMatches.begin();
		auto ret = *firstElemItr;
		_possibleMatches.erase(firstElemItr);
		return ret;
	}

	// when playing with the same instance multiple games, need to be careful of deadlocks
	for (auto i = 0 ; true ; i++)
	{
		std::lock_guard<std::mutex> guard(matches_lock);
		if (_possibleMatches.empty())
		{
			break;
		}
		i %= _possibleMatches.size();

		if (_possibleMatches[i].first->match_lock.try_lock())
		{
			if (_possibleMatches[i].second->match_lock.try_lock())
			{
				// both players are now locked and can't participate in any other match
				// it's up to the thread to unlock after game is finished
				auto ret = _possibleMatches[i];
				_possibleMatches.erase(_possibleMatches.begin() + i);
				return ret;
			}
			_possibleMatches[i].first->match_lock.unlock();
		}
	}

	return noMoreMatches;
}

bool MatchGenerator::IsValidMatch(const AlgoData* playerAptr, const AlgoData* playerBptr)
{
	return playerAptr != nullptr && playerBptr != nullptr;
}

void MatchGenerator::updateHouseEntry(const std::string& name, const GameResult& result, int player)
{
	std::lock_guard<std::mutex> guard(score_lock);
	_houseEntries.at(name).Update(result, player);
}
