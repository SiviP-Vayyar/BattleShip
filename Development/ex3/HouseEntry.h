#pragma once

#include "AlgoData.h"
#include "GameBoard.h"
#include "GameResult.h"

class HouseEntry
{
public:
	HouseEntry(int _wins=0, int _losses=0, int _ptsFor=0, int _ptsAgainst=0)
	: wins(_wins), losses(_losses), ptsFor(_ptsFor), ptsAgainst(_ptsAgainst) {}
	explicit HouseEntry(const AlgoData& data_, int _wins=0, int _losses=0, int _ptsFor=0, int _ptsAgainst=0)
	: HouseEntry(_wins, _losses, _ptsFor, _ptsAgainst) { data = data_; }
	HouseEntry(const HouseEntry& other) 
	: HouseEntry(other.data, other.wins, other.losses, other.ptsFor, other.ptsAgainst) {}
	std::string GetTeamName() const	{ return data.name; }
	void Update(const GameResult& result, int player);
	
	AlgoData data;
	int wins;
	int losses;
	int ptsFor;
	int ptsAgainst;

	struct Compare
	{
		//returns ​true if the first argument is less than (i.e. is ordered before) the second. 
		bool operator()(std::pair<std::string, HouseEntry> const & a, std::pair<std::string, HouseEntry> const & b) const
		{
			if(a.second.wins != b.second.wins)
			{
				return a.second.wins > b.second.wins; // more wins
			}
			if(a.second.losses != b.second.losses) // same wins
			{
				return a.second.wins < b.second.wins; // less losses
			}
			if(a.second.ptsFor != b.second.ptsFor) // same losses
			{
				return a.second.ptsFor > b.second.ptsFor; // more points for
			}
			if(a.second.ptsAgainst != b.second.ptsAgainst) // same points for
			{
				return a.second.ptsAgainst < b.second.ptsAgainst; // less points against
			}
			return a.second.GetTeamName() > b.second.GetTeamName(); // Alphabetically			
		}
	};
};
