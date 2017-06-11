#pragma once

#include "AlgoData.h"
#include "GameBoard.h"
#include "GameResult.h"

class HouseEntry
{
public:
	explicit HouseEntry(const AlgoData& data_, int _wins=0, int _losses=0, int _ptsFor=0, int _ptsAgainst=0, double _time = 0.0)
	: data(data_), wins(_wins), losses(_losses), ptsFor(_ptsFor), ptsAgainst(_ptsAgainst), time(_time) {}
	HouseEntry(const HouseEntry& other) 
	: HouseEntry(other.data, other.wins, other.losses, other.ptsFor, other.ptsAgainst, other.time) {}
	std::string GetTeamName() const	{ return data.name; }
	void Update(const GameResult& result, int player);
	
	const AlgoData& data;
	int wins;
	int losses;
	int ptsFor;
	int ptsAgainst;
	double time;

	struct Compare
	{
		//returns ​true if the first argument is less than (i.e. is ordered before) the second. 
		bool operator()(HouseEntry const & a, HouseEntry const & b) const
		{
			if(a.wins != b.wins)
			{
				return a.wins > b.wins; // more wins
			}
			if(a.losses != b.losses) // same wins
			{
				return a.wins < b.wins; // less losses
			}
			if(a.ptsFor != b.ptsFor) // same losses
			{
				return a.ptsFor > b.ptsFor; // more points for
			}
			if(a.ptsAgainst != b.ptsAgainst) // same points for
			{
				return a.ptsAgainst < b.ptsAgainst; // less points against
			}
			return a.GetTeamName() > b.GetTeamName(); // Alphabetically			
		}
	};
};
