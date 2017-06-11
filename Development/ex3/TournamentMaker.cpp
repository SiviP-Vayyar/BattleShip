#include "TournamentMaker.h"

#include "GameUtils.h"
#include <iostream>
#include "PrintHandler.h"
#include <map>
#include "GameException.h"
#include "GameBoardData.h"
#include <thread>
#include "MatchGenerator.h"
#include <algorithm>
#include <xatomic.h>

TournamentMaker::TournamentMaker(int argc, char* argv[])
{
	// (1) Validate input & Set input arguments
	if(!ParseInput(argc, argv))
	{
		_initSuccess = false;
		return;
	}

	//Init Logger
	Logger::Init(_inputFolder + "/game.log");

	// (2) Set all board & Validate them + Get algorithm file paths
	if(!SetAndValidateBoardsAndAlgos())
	{
		_initSuccess = false;
		return;
	}

	// (3-6) Load and Init dll
	if(!LoadAndInitAlgos())
	{
		_initSuccess = false;
		return;
	}
}

TournamentMaker::~TournamentMaker()
{
	for (auto& data : _algoDataVec)
	{
		if (data->handle != nullptr)
		{
			try
			{
				FreeLibrary(data->handle);
			}
			catch(...){ }
		}
	}
}

/*Validate input, parse it, and set all needed local variables*/
bool TournamentMaker::ParseInput(int argc, char* argv[])
{
	std::string path = ".";

	if(argc > 3) // at most 3 args - name, path, -threads
	{
		//In case more that 1 argument was given - we choose to stop the program
		throw std::exception("Program takes at most 2 arguments!");
	}

	// set path and print parameters according to argv
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-threads") == 0)
		{
			_threadLimit = std::stoi(argv[i+1]);
		}
		else
		{
			path = argv[i];
		}
	}

	// Check if wrong path
	if(GameUtils::isDirectory(path))
	{
		_inputFolder = path;
		return true;
	}

	Logger::log("Wrong path: " + path, Error);
	return false;
}

bool TournamentMaker::SetAndValidateBoardsAndAlgos()
{
	bool misGoodBoard, misAlgo, misBoards;
	misGoodBoard = misAlgo = misBoards = true;
	size_t algoCnt = 0;
	
	HANDLE dir;
	WIN32_FIND_DATAA fileData;
	std::vector<std::string> badBoardsPaths;

	// test for empty directory
	std::string s = _inputFolder + "/*";

	dir = FindFirstFileA(s.c_str(), &fileData);
	if(dir != INVALID_HANDLE_VALUE)
	{
		// test each file suffix and set variables as needed
		do
		{
			std::string fileName(fileData.cFileName);
			std::string fullFileName = _inputFolder + "/" + fileName;

			if(GameUtils::endsWith(fileName, ".sboard"))
			{
				misBoards = false;
				_boardFilePath = fullFileName;
				GameBoard newBoard(fullFileName);
				auto errors = ValidateBoard(newBoard);
				if (!errors)
				{
					misGoodBoard = false;
					_boardsVec.push_back(std::move(newBoard));
				}
				else
				{
					badBoardsPaths.push_back(fullFileName);
				}
			}
			else if(GameUtils::endsWith(fileName, ".dll"))
			{
				misAlgo = (++algoCnt) < 2;
				_dllNamesVec.push_back(fullFileName);
			}
		} while(FindNextFileA(dir, &fileData));
	}
	FindClose(dir);

	/*Validate input by an exact order*/
	if(misGoodBoard || misAlgo || misBoards)
	{
		if(misBoards)
		{
			Logger::log("No board files (*.sboard) looking in path: " + _inputFolder, Error);
		}
		if(misAlgo)
		{
			Logger::log("Missing algorithm (dll) files looking in path: " + _inputFolder, Error);
		}
		if(misGoodBoard)
		{
			Logger::log("Missing good board file (*.sboard) looking in path: " + _inputFolder, Error);
		}
		return false;
	}

	for (auto badBoard : badBoardsPaths)
	{
		Logger::log("Bad board file in path: " + badBoard, Warning);
	}

	return true;
}

bool TournamentMaker::LoadAndInitAlgos()
{
	bool failedLoadPlayer, failedGetPlayer, failedInitPlayer;
	GameBoard dummyBoard(GameBoard::newEmptyRawBoard(DUMMY_DIMENTION, DUMMY_DIMENTION, DUMMY_DIMENTION), DUMMY_DIMENTION, DUMMY_DIMENTION, DUMMY_DIMENTION); //TODO: better way?
	std::vector<std::string> logBuffer;

	for(auto& algoFile : _dllNamesVec)
	{
		AlgoData* data = new AlgoData();
		auto dataShared = std::shared_ptr<AlgoData>(data);
		data->algoFile = algoFile;
		failedLoadPlayer = failedGetPlayer = failedInitPlayer = true;
		data->name = GameUtils::GetTeamNameFromFileName(data->algoFile);

		Logger::log("Initializing: " + data->name);

		// Load dynamic library
		data->handle = LoadLibraryA(data->algoFile.c_str()); // Notice: Unicode compatible version of LoadLibrary
		if(data->handle)
		{
			failedLoadPlayer = false;
			// GetAlgorithm function - assume not thread safe when loading. use only GetPlayer as thread safe wrapper.
			data->GetPlayerUnsafe = reinterpret_cast<GetAlgoFuncType>(GetProcAddress(data->handle, GET_ALGORITHM_STR));
			if(data->GetPlayerUnsafe)
			{
				try
				{
					failedGetPlayer = false;
					// See if player is valid - so he can enter the tournament
					IAlgo* player = data->AcquirePlayer();

					// Set boards for player - Sanity Check
					GameBoardData boardData(PLAYER_A, dummyBoard);
					player->setPlayer(PLAYER_A);
					player->setBoard(boardData);
					data->ReleasePlayer();
					data->ClearPlayer();
					failedInitPlayer = false;
				}
				catch(std::exception ex)
				{
					logBuffer.push_back("Exception handling player: " + data->algoFile + "\tWhat: " + ex.what());
				}
			}
			else
			{
				logBuffer.push_back("Failed resolving GetAlgorithm: " + data->algoFile);
			}
			if(failedGetPlayer || failedInitPlayer)
			{
				if (failedGetPlayer)
				{
					logBuffer.push_back(data->name +  ": failedGetPlayer");
				}
				if(failedInitPlayer)
				{
					logBuffer.push_back(data->name + ": failedInitPlayer");
				}
				
				try
				{
					FreeLibrary(data->handle);
				}
				catch(std::exception ex)
				{
					logBuffer.push_back("Exception FreeLibrary: " + data->algoFile + "\tWhat: " + ex.what());
				}
			}
		}
		else
		{
			logBuffer.push_back("Cannot load dll: " + data->algoFile);
		}

		if(!(failedLoadPlayer || failedGetPlayer || failedInitPlayer)) // Good player
		{
			_algoDataVec.push_back(dataShared);
		}
	}

	// Print accumulated errors
	for (auto& err : logBuffer)
	{
		Logger::log(err), Error;
	}

	return !_algoDataVec.empty();
}

// run all house games multithreaded and keep score
void TournamentMaker::RunGames(const GameBoard& board, MatchGenerator* matches)
{
	for (auto match = matches->GetNextMatch(); matches->IsValidMatch(match.first, match.second); match = matches->GetNextMatch())
	{
		auto& dataA = *(match.first);
		auto& dataB = *(match.second);
		GameResult result = RunGame(*dataA, *dataB, board);
		if (_gameModeReuse)
		{
			dataA->match_lock.unlock();
			dataB->match_lock.unlock();
		}
		matches->updateHouseEntry(dataA->name, result, PLAYER_A);
		matches->updateHouseEntry(dataB->name, result, PLAYER_B);
	}
}

/* Each player playes each player in the same house twice
 * 1) As player A
 * 2) As Player B
 * This way he has no advantage in starting against him
 */
void TournamentMaker::PlayHouse(const std::vector<std::shared_ptr<AlgoData>>& house) const
{
	// match generator handles player matching and scoring thread safe
	MatchGenerator matches(house, _gameModeReuse);

	// Prelimineries - Play all house games
	for(const auto& board : _boardsVec)
	{
		Logger::log("Now playing on board: " + board.GetName() + " ...");

		Sleep(HOUSE_PRINT_INTERVAL);
		std::vector<std::thread> workers;
		workers.reserve(_threadLimit);
		for (int i = 0 ; i < _threadLimit ; i++)
		{
			workers.push_back(std::thread(RunGames, board, &matches));
		}
		for (auto& worker : workers)
		{
			worker.join();
		}
		matches.ResetIterators();

		// NOTICE: parallelism is currently limited for each round, no need for thread safety in score printing
		PrintHandler::cleanOutput();
		PrintHandler::PrintHouseStandings(matches._houseEntries);
	}	
}

void TournamentMaker::RunTournament() const
{
	if (_algoDataVec.size() < MIN_PLAYERS)
	{
		std::string err("Not enough good players for the preliminaries: " + std::to_string(_algoDataVec.size()) + " Players");
		throw std::runtime_error(err);
	}

	PlayHouse(_algoDataVec);
}

GameResult TournamentMaker::RunGame(const std::shared_ptr<AlgoData>& playerAData, const std::shared_ptr<AlgoData>& playerBData, const GameBoard& gameBoard)
{
	GameResult result;
	IAlgo *playerA, *playerB;
	playerA = playerB = nullptr;
	bool techLossA, techLossB;
	techLossA = techLossB = false;
	auto rawBoardA = gameBoard.getBoardForPlayer(PLAYER_A);
	auto rawBoardB = gameBoard.getBoardForPlayer(PLAYER_B);

	try
	{
		playerA = playerAData->AcquirePlayer();
		GameBoardData boardData(PLAYER_A, gameBoard);
		playerA->setPlayer(PLAYER_A);
		playerA->setBoard(boardData);
	}
	catch(...)
	{
		techLossA = true;
	}
	
	try
	{
		playerB = playerBData->AcquirePlayer();
		GameBoardData boardData(PLAYER_B, gameBoard);
		playerB->setPlayer(PLAYER_B);
		playerB->setBoard(boardData);
	}
	catch(...)
	{
		techLossB = true;
	}
	
	if (techLossA != techLossB)
	{
		if (techLossA)
		{
			result.scoreB = gameBoard.GetMaxScore(PLAYER_B);
		}
		else
		{
			result.scoreA = gameBoard.GetMaxScore(PLAYER_A);
		}
	}
	if (!techLossA || !techLossB) //if both did load correctly
	{
		//We have both players - let's start playing:
		GameMaker maker(playerA, playerB, gameBoard, playerAData->name, playerBData->name);
		result = maker.RunGame();
	}

	return result;
}

GameBoard::BoardErrors TournamentMaker::ValidateBoard(const GameBoard& gameBoard)
{
	GameBoard::BoardErrors boardErrors;

	GameBoard copyBoard(gameBoard);

	// shipsData = pair<int, set<char>>
	auto shipsDataA = copyBoard.analyseShips(PLAYER_A);
	auto shipsDataB = copyBoard.analyseShips(PLAYER_B);

	// find out if there are ships with wrong size on the board
	auto& illegalShipsA = shipsDataA.second;
	auto& illegalShipsB = shipsDataB.second;
	boardErrors.wrongSizeA = !illegalShipsA.empty();
	boardErrors.wrongSizeB = !illegalShipsB.empty();

	// count number of ships for each player
	int numShipsA = shipsDataA.first;
	int numShipsB = shipsDataB.first;
	boardErrors.manyA = (numShipsA > MAX_SHIPS);
	boardErrors.fewA = (numShipsA < MAX_SHIPS);
	boardErrors.manyB = (numShipsB > MAX_SHIPS);
	boardErrors.fewB = (numShipsB < MAX_SHIPS);

	// detect illegal ship placement (adjacent ships) - work on full board
	boardErrors.adjacent = gameBoard.isAdjacent();

	return boardErrors;
}
