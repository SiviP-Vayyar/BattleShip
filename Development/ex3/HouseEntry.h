#pragma once

#include "TournamentMaker.h"

class HouseEntry
{
public:
	HouseEntry() : wins(0), losses(0), ptsFor(0), ptsAgainst(0)
	{}
	explicit HouseEntry(const AlgoData& data_) : HouseEntry()
	{
		data = data_;
	}
	AlgoData data;
	int wins;
	int losses;
	int ptsFor;
	int ptsAgainst;
	std::string GetTeamName() const
	{
		return data.name;
	}
	void Update(const GameResult& result, int player);

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
