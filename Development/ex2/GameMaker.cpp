#include "GameMaker.h"
#include "GameException.h"
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <winapifamily.h>
#include "GameUtils.h"


GameMaker::GameMaker(int argc, char* argv[])
{
	// (1) Validate input & Set input arguments
	if (!ParseInput(argc, argv))
	{
		throw GameException("");
	}

	// (2) Set local Boards & Validate them + Validate algorithm files
	if (!SetAndValidateBoardsAndAlgos())
	{
		throw GameException("");
	}

	// (3-6) Load and Init dll
	if (!LoadAndInitAlgos())
	{
		throw GameException("");
	}
	
	// Set boards for both players
	char** rawBoardA = _board.getBoardForPlayer(PLAYER_A);
	char** rawBoardB = _board.getBoardForPlayer(PLAYER_B);
	_playerA->setBoard(PLAYER_A ,const_cast<const char**>(rawBoardA), _board.rows(), _board.cols());
	_playerB->setBoard(PLAYER_B, const_cast<const char**>(rawBoardB), _board.rows(), _board.cols());
	GameBoard::deleteRawBoard(rawBoardA, _board.rows(), _board.cols());
	GameBoard::deleteRawBoard(rawBoardB, _board.rows(), _board.cols());
}

/*@pre: assume players and board were set and validated*/
void GameMaker::RunGame() //TODO: ZOHAR - I think it's ok but just make sure. all I did was change Player& to IAlgo*
{
	// counters and flags
	int scorePlayerA = 0, scorePlayerB = 0;
	int remainingShipsA = MAX_SHIPS, remainingShipsB = MAX_SHIPS;
	bool movesRemainingA = true, movesRemainingB = true;

	// alternating variables for game loop
	auto currentPlayerDef = PLAYER_A;
	auto currentPlayer = _playerA;
	auto currentPlayerMovesRemaining = &movesRemainingA;
	auto currentShipsCntr = &remainingShipsA;
	auto opponentShipsCntr = &remainingShipsB;
	auto currentScore = &scorePlayerA;
	auto opponentScore = &scorePlayerB;

	// game loop
	while (remainingShipsA > 0 && remainingShipsB > 0 && (movesRemainingA || movesRemainingB))
	{
		// get attack from player and play it on the board
		auto attackPosition = currentPlayer->attack();
		*currentPlayerMovesRemaining = (attackPosition != ATTACK_END);
		auto attackResultInfo = _board.attack(attackPosition);
		auto attackResult = attackResultInfo.first;
		char attackedPiece = attackResultInfo.second;
		bool selfHit = GameBoard::isPlayerShip(currentPlayerDef, attackedPiece);
		int row = attackPosition.first, col = attackPosition.second;

		// notify the players:
		_playerA->notifyOnAttackResult(currentPlayerDef, row, col, attackResult);
		_playerB->notifyOnAttackResult(currentPlayerDef, row, col, attackResult);

		// upon hit player gets another turn (if he didn't shoot himself)
		if (attackResult == AttackResult::Hit && !selfHit)
		{
			continue;
		}
		// upon sink player gets another turn (if he didn't shoot himself) + update ships counter and score
		if (attackResult == AttackResult::Sink)
		{
			auto shipsCntr = selfHit ? currentShipsCntr : opponentShipsCntr;
			auto score = selfHit ? opponentScore : currentScore;
			*shipsCntr = *shipsCntr - 1;
			*score = *score + GameBoard::getShipScore(attackedPiece);
			if (!selfHit)
				continue;
		}

		// switch players for next round
		currentPlayerDef = currentPlayerDef == PLAYER_A ? PLAYER_B : PLAYER_A;
		currentPlayer = currentPlayer == _playerA ? std::move(_playerB) : std::move(_playerA); //TODO: make sure move works
		currentPlayerMovesRemaining = currentPlayerMovesRemaining == &movesRemainingA ? &movesRemainingB : &movesRemainingA;
		std::swap(currentShipsCntr, opponentShipsCntr);
		std::swap(currentScore, opponentScore);
	}

	// print end game results
	if (remainingShipsA == 0 || remainingShipsB == 0)
	{
		std::cout << "Player " << (remainingShipsA == 0 ? 'B' : 'A') << " won" << std::endl;
	}
	std::cout << "Points:" << std::endl;
	std::cout << "Player A: " << scorePlayerA << std::endl;
	std::cout << "Player B: " << scorePlayerB << std::endl;
}



/*Validate input, parse it, and set all needed local variables*/
bool GameMaker::ParseInput(int argc, char* argv[]) //TODO: make sure order of prints after change!!!
{
	std::string path = "";
	
	// set path according to argv
	if (argc == 1) //TODO: support prints to console
	{
		path = ".";
	}
	else if (argc == 2)
	{
		path = argv[1];
	}
	else //In too many argument were given - we choose to ignore
	{
		//TODO: -quiet
	}

	// Check if wrong path
	if (isDirectory(path))
	{
		_inputFolder = path;
		return true;
	}

	std::cout << "Wrong path: " << path.c_str() << std::endl;
	return false;	
}

bool GameMaker::LoadAndInitAlgos()
{
	bool failedLoadA, failedLoadB, failedInitA, failedInitB;
	failedLoadA = failedLoadB = failedInitA = failedInitB = true;

	HINSTANCE hDll;

	// Load dynamic library - Algo A
	hDll = LoadLibraryA(_algoFileA.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (hDll)
	{
		// GetAlgorithm function
		auto GetAlgorithm = reinterpret_cast<GetAlgoFuncType>(GetProcAddress(hDll, GET_ALGORITHM_STR));
		if (GetAlgorithm)
		{
			failedLoadA = false;
			_playerA = std::shared_ptr<IAlgo>(GetAlgorithm());
			failedInitA = !_playerA->init(_inputFolder);
		}
	}

	// Load dynamic library - Algo B
	hDll = LoadLibraryA(_algoFileB.c_str()); // Notice: Unicode compatible version of LoadLibrary
	if (hDll)
	{
		// GetAlgorithm function
		auto GetAlgorithm = reinterpret_cast<GetAlgoFuncType>(GetProcAddress(hDll, GET_ALGORITHM_STR));
		if (GetAlgorithm)
		{
			failedLoadB = false;
			_playerB = std::shared_ptr<IAlgo>(GetAlgorithm());
			failedInitB = !_playerB->init(_inputFolder);
		}
	}
	
	if (failedLoadA || failedLoadB || failedInitA || failedInitB)
	{
		if (failedLoadA)
		{
			std::cout << "Cannot load dll: " << _algoFileA << std::endl;
		}
		if (failedLoadB)
		{
			std::cout << "Cannot load dll: " << _algoFileB << std::endl;
		}
		if (failedInitA)
		{
			std::cout << "Algorithm initialization failed for dll: " << _algoFileA << std::endl;
		}
		if (failedInitB)
		{
			std::cout << "Algorithm initialization failed for dll: " << _algoFileB << std::endl;
		}
		return false;
	}
	return true;
}

bool GameMaker::SetAndValidateBoardsAndAlgos()
{
	bool misBoard, misAlgo, wrongSizeA, wrongSizeB, fewA, fewB, manyA, manyB, adjacent;
	misBoard = misAlgo = wrongSizeA = wrongSizeB = fewA = fewB = manyA = manyB = adjacent = true;
	std::set<char> illegalShipsA, illegalShipsB;

	HANDLE dir;
	WIN32_FIND_DATAA fileData;
	std::vector<std::string> dllNamesVec;
	GameBoard fullBoard;

	// test for empty directory
	std::string s = _inputFolder + "\\*";

	dir = FindFirstFileA(s.c_str(), &fileData);
	if(dir != INVALID_HANDLE_VALUE)
	{
		// test each file suffix and set variables as needed
		do
		{
			std::string fileName(fileData.cFileName);

			// In case there are multiple possibilities - we choose to take the last one
			if(endsWith(fileName, ".sboard"))
			{
				std::string fullFileName = _inputFolder + "\\" + fileName;
				_boardFilePath = fullFileName;
				misBoard = false;
			}
			else if(endsWith(fileName, ".dll"))
			{
				dllNamesVec.push_back(fileName);
			}

		} while(FindNextFileA(dir, &fileData));
	}

	// Set Player A
	if(dllNamesVec.size() > 0)
	{
		_algoFileA = _inputFolder + "\\" + dllNamesVec[0];
	}

	// Set Player B
	if(dllNamesVec.size() > 1)
	{
		misAlgo = false;
		_algoFileB = _inputFolder + "\\" + dllNamesVec[1];
	}

	if(!misBoard)
	{
		fullBoard = GameBoard(_boardFilePath);
		GameBoard copyBoard(fullBoard);

		// shipsData = pair<int, set<char>>
		auto shipsDataA = copyBoard.analyseShips(PLAYER_A);
		auto shipsDataB = copyBoard.analyseShips(PLAYER_B);

		// find out if there are ships with wrong size on the board
		illegalShipsA = shipsDataA.second;
		illegalShipsB = shipsDataB.second;
		wrongSizeA = !illegalShipsA.empty();
		wrongSizeB = !illegalShipsB.empty();

		// count number of ships for each player
		int numShipsA = shipsDataA.first;
		int numShipsB = shipsDataB.first;
		manyA = (numShipsA > MAX_SHIPS);
		fewA = (numShipsA < MAX_SHIPS);
		manyB = (numShipsB > MAX_SHIPS);
		fewB = (numShipsB < MAX_SHIPS);

		// detect illegal ship placement (adjacent ships) - work on full board
		adjacent = fullBoard.isAdjacent();
	}

	

	/*Validate input by an exact order*/
	if (wrongSizeA || wrongSizeB || fewA || fewB || manyA || manyB || adjacent)
	{
		if(misBoard)
		{
			std::cout << "Missing board file (*.sboard) looking in path: " << _inputFolder.c_str() << std::endl;
		}
		else // only if board exists
		{
			if(wrongSizeA)
			{
				//print a line for each wrong size type
				for(char shipType : illegalShipsA)
				{
					std::cout << "Wrong size or shape for ship " << shipType << " for player A" << std::endl;
				}
			}
			if(wrongSizeB)
			{
				//print a line for each wrong size type
				for(char shipType : illegalShipsB)
				{
					std::cout << "Wrong size or shape for ship " << shipType << " for player B" << std::endl;
				}
			}
			if(manyA)
			{
				std::cout << "Too many ships for player A" << std::endl;
			}
			if(fewA)
			{
				std::cout << "Too few ships for player A" << std::endl;
			}
			if(manyB)
			{
				std::cout << "Too many ships for player B" << std::endl;
			}
			if(fewB)
			{
				std::cout << "Too few ships for player B" << std::endl;
			}
			if(adjacent)
			{
				std::cout << "Adjacent Ships on Board" << std::endl;
			}
		}
		if (misAlgo)
		{
			std::cout << "Missing an algorithm (dll) file looking in path: " << _inputFolder.c_str() << std::endl;
		}
		return false;
	}

	// set local board
	char** rawBoard = fullBoard.getBoard();
	_board = GameBoard(rawBoard, fullBoard.rows(), fullBoard.cols());
	GameBoard::deleteRawBoard(rawBoard, fullBoard.rows(), fullBoard.cols());

	return true;
}
