#pragma once

#include "IBattleshipGameAlgo.h"
#include <algorithm>
#include "GameMaker.h"
#include <string>
#include <windows.h>

typedef IAlgo*(*GetAlgoFuncType)();

#define GET_ALGORITHM_STR "GetAlgorithm"
#define MIN_PLAYERS 32
#define MIN_HOUSES (MIN_PLAYERS/2)
#define DEFAULT_BEST_OF 3

struct AlgoData
{
	std::string name;
	std::string algoFile;
	GetAlgoFuncType GetPlayer;
	HINSTANCE handle;
};

class HouseEntry
{
public:
	explicit HouseEntry(const AlgoData& data_) : data(data_), wins(0), losses(0), ptsFor(0), ptsAgainst(0)
	{}
	const AlgoData& data;
	int wins;
	int losses;
	int ptsFor;
	int ptsAgainst;
	std::string GetTeamName() const
	{
		return data.name;
	}
	void Update(const GameResult& result, int player)
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
	template< class RandomIt>
	static void Sort(RandomIt first, RandomIt last)
	{
		// returns ​true if the first argument is less than (i.e. is ordered before) the second. 
		auto cmp = [](std::pair<std::string, HouseEntry&> const & a, std::pair<std::string, HouseEntry&> const & b)
		{
			if(a.second.wins != b.second.wins)
			{
				return a.second.wins < b.second.wins; // more wins
			}
			else if(a.second.losses != b.second.losses) // same wins
			{
				return a.second.wins > b.second.wins; // less losses
			}
			else if(a.second.ptsFor != b.second.ptsFor) // same losses
			{
				return a.second.ptsFor < b.second.ptsFor; // more points for
			}
			else if(a.second.ptsAgainst != b.second.ptsAgainst) // same points for
			{
				return a.second.ptsAgainst > b.second.ptsAgainst; // less points against
			}
			else
			{
				return a.second.GetTeamName() < b.second.GetTeamName(); // Alphabetically
			}
		};
		std::sort(first, last, cmp);
	}
};


class TournamentMaker
{
public:
	TournamentMaker(int argc, char* argv[]);
	~TournamentMaker();

	TournamentMaker(const TournamentMaker&) = delete;
	TournamentMaker& operator= (const TournamentMaker&) = delete;
	std::vector<std::vector<AlgoData>> DividePlayersToHouses(int numOfHouses = MIN_HOUSES);
	std::pair<AlgoData, AlgoData> GetWinnersFromHouse(const std::vector<AlgoData>& house);
	std::vector<AlgoData> PlayTournamentStage(const std::vector<AlgoData>& stagePlayers, size_t bestOf = DEFAULT_BEST_OF);
	void RunTournament();
	GameResult RunGame(const AlgoData& playerAData, const AlgoData& playerBData, const GameBoard& gameBoard);
	GameBoard GetNextBoard() {return _boardsVec[_currBoardIdx++ % _boardsVec.size()]; }
	static GameBoard::BoardErrors ValidateBoard(const GameBoard& gameBoard);

	bool isInitSuccess() const { return _initSuccess; }

private:
	std::string _inputFolder;
	std::string _boardFilePath;
	std::vector<AlgoData> _algoDataVec;
	std::vector<GameBoard> _boardsVec;
	size_t _currBoardIdx;
	std::vector<std::string> _dllNamesVec;

	bool _initSuccess;

	bool ParseInput(int argc, char* argv[]);
	bool SetAndValidateBoardsAndAlgos();
	bool LoadAndInitAlgos();
};