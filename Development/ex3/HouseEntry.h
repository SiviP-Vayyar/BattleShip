#pragma once

#include "AlgoData.h"
#include "GameResult.h"

class HouseEntry
{
public:
	HouseEntry() : HouseEntry("") {}
	explicit HouseEntry(
		const std::string& name_,
		int _wins=0,
		int _losses=0,
		int _ptsFor=0,
		int _ptsAgainst=0,
		double _time = 0.0)
		: name(name_)
		, wins(_wins)
		, losses(_losses)
		, ptsFor(_ptsFor)
		, ptsAgainst(_ptsAgainst)
		, time(_time) {}

	HouseEntry& operator=(const HouseEntry& other) = default;
	std::string GetTeamName() const	{ return name; }
	void Update(const GameResult& result, int player);
	
	std::string name;
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
