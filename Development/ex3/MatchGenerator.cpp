#include "MatchGenerator.h"


MatchGenerator::MatchGenerator(const std::vector<std::shared_ptr<AlgoData>>& house): _players(house)
{
	ResetIterators();
	for (auto& data : house)
	{
		_houseEntries[data->name] = HouseEntry(data->name);
	}
}

std::pair<std::vector<std::shared_ptr<AlgoData>>::const_iterator, std::vector<std::shared_ptr<AlgoData>>::const_iterator> MatchGenerator::GetNextMatch()
{
	std::lock_guard<std::mutex> guard(matches_lock);
	auto ret = std::make_pair(_playerAiter, _playerBiter);

	if (_playerAiter != _players.cend())
	{
		++_playerAiter;
	}
	if (_playerAiter == _players.cend())
	{
		if (_playerBiter != _players.cend())
		{
			++_playerBiter;
		}
		if (_playerBiter != _players.cend())
		{
			_playerAiter = _players.cbegin();
		}
	}

	return ret;
}

boolean MatchGenerator::IsValidMatch(const std::vector<std::shared_ptr<AlgoData>>::const_iterator& playerAiter, const std::vector<std::shared_ptr<AlgoData>>::const_iterator& playerBiter) const
{
	return playerAiter != _players.cend() && playerBiter != _players.cend();
}

void MatchGenerator::updateHouseEntry(const std::string& name, const GameResult& result, int player)
{
	std::lock_guard<std::mutex> guard(score_lock);
	_houseEntries.at(name).Update(result, player);
}
