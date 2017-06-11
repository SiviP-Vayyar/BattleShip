#pragma once

#include "AlgoData.h"
#include "HouseEntry.h"
#include <mutex>
#include "MatchGenerator.h"



#define GET_ALGORITHM_STR "GetAlgorithm"
#define MIN_PLAYERS 2
#define DEFAULT_BEST_OF 3
#define PLAYING_ROUNDS 25
#define DEFAULT_THREAD_LIMIT 4
#define HOUSE_PRINT_INTERVAL 500

class TournamentMaker
{
public:
	TournamentMaker(int argc, char* argv[]);
	~TournamentMaker();

	TournamentMaker(const TournamentMaker&) = delete;
	TournamentMaker& operator= (const TournamentMaker&) = delete;
	void PlayHouse(const std::vector<std::shared_ptr<AlgoData>>& house) const;
	void RunTournament() const;

	static void RunGames(const GameBoard& board, MatchGenerator* matches);
	static GameResult RunGame(const std::shared_ptr<AlgoData>& playerAData, const std::shared_ptr<AlgoData>& playerBData, const GameBoard& gameBoard);
	static GameBoard::BoardErrors ValidateBoard(const GameBoard& gameBoard);

	bool isInitSuccess() const { return _initSuccess; }

private:
	std::string _inputFolder;
	std::string _boardFilePath;
	std::vector<std::shared_ptr<AlgoData>> _algoDataVec;
	std::vector<GameBoard> _boardsVec;
	std::vector<std::string> _dllNamesVec;

	bool _initSuccess;
	size_t _threadLimit = DEFAULT_THREAD_LIMIT;

	bool ParseInput(int argc, char* argv[]);
	bool SetAndValidateBoardsAndAlgos();
	bool LoadAndInitAlgos();
};