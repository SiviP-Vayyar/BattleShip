#include "HouseEntry.h"

void HouseEntry::Update(const GameResult& result, int player)
{
	if(player == PLAYER_A)
	{
		ptsFor += result.scoreA;
		ptsAgainst += result.scoreB;
		if(result.Winner() == PLAYER_A)
		{
			++wins;
		}
		else if(result.Winner() == PLAYER_B)
		{
			++losses;
		}
	}
	else // PLAYER_B
	{
		ptsFor += result.scoreB;
		ptsAgainst += result.scoreA;
		if(result.Winner() == PLAYER_B)
		{
			++wins;
		}
		else if(result.Winner() == PLAYER_A)
		{
			++losses;
		}
	}
}