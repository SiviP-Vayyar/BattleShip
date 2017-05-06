#include "TournamentMaker.h"

#include "GameUtils.h"
#include <iostream>
#include "PrintHandler.h"
#include <map>
#include "GameException.h"

TournamentMaker::TournamentMaker(int argc, char* argv[])
{
	// (1) Validate input & Set input arguments
	if(!ParseInput(argc, argv))
	{
		_initSuccess = false;
		return;
	}

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
		if (data.handle != nullptr)
		{
			try
			{
				FreeLibrary(data.handle);
			}
			catch(...){ }
		}
	}
}

/*Validate input, parse it, and set all needed local variables*/
bool TournamentMaker::ParseInput(int argc, char* argv[])
{
	std::string path = ".";
	bool printEnabled = PRINT_ENABLED_DEFAULT;
	int printDelay = PRINT_DELAY_DEFAULT;

	if(argc > 5) // at most 5 args - name, path, -quiet, -delay + amount
	{
		//In case more that 1 argument was given - we choose to stop the program
		throw std::exception("Program takes at most 4 arguments!");
	}

	// set path and print parameters according to argv
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-quiet") == 0)
		{
			printEnabled = false;
		}
		else if(strcmp(argv[i], "-delay") == 0)
		{
			i++;
			if(i == argc)
			{
				throw std::exception("Missing delay amount argument!");
			}
			char* endPtr;
			printDelay = int(strtol(argv[i], &endPtr, 10));
			if(*endPtr)
			{
				throw std::exception("Delay argument must be an integer!");
			}
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
		PrintHandler::init(printEnabled, printDelay);
		return true;
	}

	std::cout << "Wrong path: " << path.c_str() << std::endl;
	return false;
}

bool TournamentMaker::SetAndValidateBoardsAndAlgos()
{
	bool misGoodBoard, misAlgo;
	misGoodBoard = misAlgo = true;
	
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
				misAlgo = false;
				_dllNamesVec.push_back(fullFileName);
			}
		} while(FindNextFileA(dir, &fileData));
	}
	FindClose(dir);

	/*Validate input by an exact order*/
	if(misGoodBoard || misAlgo)
	{
		if(misGoodBoard)
		{
			std::cout << "Missing good board file (*.sboard) looking in path: " << _inputFolder.c_str() << std::endl;
		}
		if(misAlgo)
		{
			std::cout << "Missing an algorithm (dll) file looking in path: " << _inputFolder.c_str() << std::endl;
		}
		return false;
	}

	for (auto badBoard : badBoardsPaths)
	{
		std::cout << "Bad board file in path: " << badBoard << std::endl;
	}

	return true;
}

bool TournamentMaker::LoadAndInitAlgos()
{
	bool failedLoadPlayer, failedGetPlayer, failedInitPlayer;
	char** emptyBoard = GameBoard::newEmptyRawBoard(BOARD_ROWS, BOARD_COLS);
	std::vector<std::string> logBuffer;

	for(auto algoFile : _dllNamesVec)
	{
		AlgoData data;
		data.algoFile = algoFile;
		failedLoadPlayer = failedGetPlayer = failedInitPlayer = true;
		data.name = GameUtils::GetTeamNameFromFileName(data.algoFile);

		// Load dynamic library
		data.handle = LoadLibraryA(data.algoFile.c_str()); // Notice: Unicode compatible version of LoadLibrary
		if(data.handle)
		{
			failedLoadPlayer = false;
			// GetAlgorithm function
			data.GetPlayer = reinterpret_cast<GetAlgoFuncType>(GetProcAddress(data.handle, GET_ALGORITHM_STR));
			if(data.GetPlayer)
			{
				try
				{
					failedGetPlayer = false;
					// See if player is valid - so he can enter the tournament
					IAlgo* player = data.GetPlayer();

					// Set boards for player - Sanity Check
					player->setBoard(PLAYER_A, const_cast<const char**>(emptyBoard), BOARD_ROWS, BOARD_COLS);
					// Init player - Sanity Check
					failedInitPlayer = !player->init(_inputFolder);
					delete player;
				}
				catch(std::exception ex)
				{
					logBuffer.push_back("Exception handling player: " + data.algoFile + "\tWhat: " + ex.what());
				}
			}
			else
			{
				logBuffer.push_back("Failed resolving GetAlgorithm: " + data.algoFile);
			}
			if(failedGetPlayer || failedInitPlayer)
			{
				try
				{
					FreeLibrary(data.handle);
				}
				catch(std::exception ex)
				{
					logBuffer.push_back("Exception FreeLibrary: " + data.algoFile + "\tWhat: " + ex.what());
				}
			}
		}
		else
		{
			logBuffer.push_back("Cannot load dll: " + data.algoFile);
		}

		if(!(failedLoadPlayer || failedGetPlayer || failedInitPlayer)) // Good player
		{
			_algoDataVec.push_back(data);
		}
	}

	GameBoard::deleteRawBoard(emptyBoard, BOARD_ROWS, BOARD_COLS);

	return !_algoDataVec.empty();
}

std::vector<std::vector<AlgoData>> TournamentMaker::DividePlayersToHouses(int numOfHouses)
{
	std::vector<std::vector<AlgoData>> houses(numOfHouses);
#ifndef _DEBUG
	std::random_shuffle(_algoDataVec.begin(), _algoDataVec.end());
#endif
	size_t idx = 0;

	for (auto& data : _algoDataVec)
	{
		houses[idx % numOfHouses].push_back(data);
		++idx;
	}
	return houses;
}

/* Each player playes each player in the same house twice
 * 1) As player A
 * 2) As Player B
 * This way he has no advantage in starting against him
 */
std::pair<AlgoData, AlgoData> TournamentMaker::GetWinnersFromHouse(const std::vector<AlgoData>& house)
{
	// Work with house entries - sorted as we decide
	std::map<std::string, HouseEntry> houseEntries;
	for (auto& data : house)
	{
		houseEntries[data.name] = HouseEntry(data);
	}

	// Prelimineries - Play all house games
	for (auto dataA = house.cbegin(); dataA != house.cend(); ++dataA)
	{
		for(auto dataB = house.cbegin(); dataB != house.cend(); ++dataB)
		{
			GameResult result = RunGame(*dataA, *dataB, GetNextBoard());
			houseEntries[dataA->name].Update(result, PLAYER_A);
			houseEntries[dataB->name].Update(result, PLAYER_B);
		}
	}

	// Sort house entries
	std::vector<std::pair<std::string, HouseEntry>> mapCopy(houseEntries.begin(), houseEntries.end());
	std::sort(mapCopy.begin(), mapCopy.end(), HouseEntry::Compare());

	// Choose first two players - Eliminate duplicates for same team
	HouseEntry firstPlace = mapCopy.begin()->second;
	for (auto entry = mapCopy.begin(); entry != mapCopy.end(); ++entry)
	{
		if (entry->second.GetTeamName() != firstPlace.GetTeamName())
		{
			return std::make_pair(firstPlace.data, entry->second.data);
		}
	}

	// If all of the same player for some reason - just return the first two
	return std::make_pair(firstPlace.data, std::next(mapCopy.begin())->second.data);
}

std::vector<AlgoData> TournamentMaker::PlayTournamentStage(const std::vector<AlgoData>& stagePlayers, size_t bestOf)
{
	std::vector<AlgoData> nextStagePlayers(stagePlayers.size() / 2);
	std::vector<GameResult> resultsVec(stagePlayers.size() / 2); // Abusing game results to count "best of" winner

	for(size_t matchIdx = 0; matchIdx < stagePlayers.size(); matchIdx += 2)
	{
		for(size_t roundIdx = 0; roundIdx < bestOf; roundIdx++)
		{
			auto result = RunGame(stagePlayers[matchIdx], stagePlayers[matchIdx + 1], GetNextBoard());
			if (result.Winner() == PLAYER_A)
			{
				resultsVec[matchIdx / 2].scoreA++;
			}
			else if (result.Winner() == PLAYER_B)
			{
				resultsVec[matchIdx / 2].scoreB++;
			}
		}
		if(resultsVec[matchIdx / 2].Winner() == PLAYER_A)
		{
			nextStagePlayers[matchIdx / 2] = stagePlayers[matchIdx];
		}
		else if(resultsVec[matchIdx / 2].Winner() == PLAYER_B)
		{
			nextStagePlayers[matchIdx / 2] = stagePlayers[matchIdx + 1];
		}
		else // In case of tie - random winner
		{
			nextStagePlayers[matchIdx / 2] = stagePlayers[matchIdx + rand() % 2];
		}
	}

	PrintHandler::PrintTournametStage(resultsVec);

	return nextStagePlayers;
}

void TournamentMaker::RunTournament()
{
	if (_algoDataVec.size() < MIN_PLAYERS)
	{
		std::cout << "Not enough players for the preliminaries: " << _algoDataVec.size() << " Players" << std::endl;
		throw GameException("");
	}

	std::vector<std::pair<AlgoData,AlgoData>> winnersVec;

	// Divide into houses
	std::vector<std::vector<AlgoData>> houses = DividePlayersToHouses();

	// Prelimineries
	for (auto& house : houses)
	{
		auto winners = GetWinnersFromHouse(house);
		winnersVec.push_back(std::move(winners));
	}

	// Top MIN_PLAYERS teams - First of house X with Second of house Y and vice versa
	std::vector<AlgoData> topPlayersVec(winnersVec.size()*2);
	for (size_t i = 0; i < winnersVec.size(); i+=2)
	{
		topPlayersVec[2 * i]	 = winnersVec[i]    .first;
		topPlayersVec[2 * i + 1] = winnersVec[i + 1].second;
		topPlayersVec[2 * i + 2] = winnersVec[i + 1].first;
		topPlayersVec[2 * i + 3] = winnersVec[i]    .second;
	}

	while (topPlayersVec.size() > 1)
	{
		topPlayersVec = PlayTournamentStage(topPlayersVec);
	}

	// We Have A Winner!!!
	PrintHandler::PrintWinner(topPlayersVec[0]);
		
}

GameResult TournamentMaker::RunGame(const AlgoData& playerAData, const AlgoData& playerBData, const GameBoard& gameBoard) const
{
	GameResult result(0, 0);
	IAlgo *playerA, *playerB;
	playerA = playerB = nullptr;
	bool techLossA, techLossB;
	techLossA = techLossB = false;
	auto rawBoard = gameBoard.getBoard();

	try
	{
		playerA = playerAData.GetPlayer();
		playerA->init(_inputFolder);
		playerA->setBoard(PLAYER_A, const_cast<const char**>(rawBoard), gameBoard.rows(), gameBoard.cols());
	}
	catch(std::exception ex)
	{
		techLossA = true;
	}
	
	try
	{
		playerB = playerBData.GetPlayer();
		playerB->init(_inputFolder);
		playerB->setBoard(PLAYER_B, const_cast<const char**>(rawBoard), gameBoard.rows(), gameBoard.cols());
	}
	catch(std::exception ex)
	{
		techLossB = true;
	}

	GameBoard::deleteRawBoard(rawBoard, gameBoard.rows(), gameBoard.cols());
	
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
		return result;
	}
	if (techLossA && techLossB) //if both didn't load correctly
	{
		return result;
	}

	//We have both players - let's start playing:
	GameMaker maker(playerA, playerB, gameBoard);
	return maker.RunGame();
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
