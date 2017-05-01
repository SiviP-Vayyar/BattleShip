#pragma once

#include "IBattleshipGameAlgo.h"
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
