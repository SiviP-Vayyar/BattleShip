#pragma once

#include "AlgoData.h"
#include "HouseEntry.h"
#include <mutex>
#include "MatchGenerator.h"



#define GET_ALGORITHM_STR "GetAlgorithm"
#define MIN_PLAYERS 2 //TODO: 32
#define MIN_HOUSES (MIN_PLAYERS/2)
#define DEFAULT_BEST_OF 3
#define PLAYING_ROUNDS 25
#define DEFAULT_THREAD_LIMIT 8

class TournamentMaker
{
public:
	TournamentMaker(int argc, char* argv[]);
	~TournamentMaker();

	TournamentMaker(const TournamentMaker&) = delete;
	TournamentMaker& operator= (const TournamentMaker&) = delete;
	std::vector<std::vector<AlgoData>> DividePlayersToHouses(int numOfHouses = MIN_HOUSES);
	std::tuple<AlgoData, AlgoData, std::vector<std::pair<std::string, HouseEntry>>> GetWinnersFromHouse(const std::vector<AlgoData>& house, size_t playingRounds = 1);
	std::vector<AlgoData> PlayTournamentStage(const std::vector<AlgoData>& stagePlayers, size_t bestOf = DEFAULT_BEST_OF);
	void RunTournament(int numOfHouses = MIN_HOUSES);
	GameBoard GetNextBoard() { std::lock_guard<std::mutex> guard(board_lock);  return _boardsVec[_currBoardIdx++ % _boardsVec.size()]; }

	static void RunGames(MatchGenerator* matches, TournamentMaker* tMaker);
	static GameResult RunGame(const AlgoData& playerAData, const AlgoData& playerBData, const GameBoard& gameBoard);
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
	size_t _threadLimit = DEFAULT_THREAD_LIMIT;
	std::mutex board_lock;

	bool ParseInput(int argc, char* argv[]);
	bool SetAndValidateBoardsAndAlgos();
	bool LoadAndInitAlgos();
};