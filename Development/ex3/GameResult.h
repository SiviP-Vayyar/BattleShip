#pragma once

struct GameResult
{
	GameResult() : scoreA(0), scoreB(0), timeA(0.0), timeB(0.0)
	{}
	int scoreA;
	int scoreB;
	double timeA;
	double timeB;
	int Winner() const
	{
		if(scoreB == scoreA)
		{
			return PLAYER_NOT_YET_KNOWN;
		}
		return scoreA > scoreB ? PLAYER_A : PLAYER_B;
	}
};