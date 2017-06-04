#pragma once

struct GameResult
{
	GameResult() : GameResult(0, 0)
	{}
	GameResult(int A, int B) : scoreA(A), scoreB(B)
	{}
	int scoreA;
	int scoreB;
	int Winner() const
	{
		if(scoreB == scoreA)
		{
			return PLAYER_NOT_YET_KNOWN;
		}
		return scoreA > scoreB ? PLAYER_A : PLAYER_B;
	}
};