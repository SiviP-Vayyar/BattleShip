#include "GameMaker.h"
#include "GameException.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <winapifamily.h>
#include <algorithm>


GameMaker::GameMaker(int argc, char* argv[])
{
	// Validate input & Set input arguments
	if (!ParseInput(argc, argv))
	{
		throw GameException("");
	}

	// Set local Boards & Validate them
	if (!SetAndValidateBoards())
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

	// Init Algorithms
	_playerA->init(_inputFolder);
	_playerB->init(_inputFolder);
}

/*@pre: assume players and board were set and validated*/
void GameMaker::RunGame() //TODO: ZOHAR
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
		bool selfHit = false;// currentPlayer->isPlayerShip(attackedPiece); //TODO: should work with Interface IBattle only!!
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
		currentPlayer = currentPlayer == _playerA ? _playerB : _playerA;
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

/*chek if the path is a valid directory*/
bool isDirectory(const std::string& path)
{
	DWORD ftyp = GetFileAttributesA(path.c_str());
	
	// test for invalid path
	if(ftyp == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	// test if path is a directory
	if(ftyp & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}
	return false;
}

/*test if str ends with suffix*/
inline bool endsWith(std::string const & str, std::string const & suffix)
{
	if(suffix.size() > str.size())
	{
		return false;
	}

	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

/*Validate input, parse it, and set all needed local variables*/
bool GameMaker::ParseInput(int argc, char* argv[]) //TODO: SIVAN
{
	bool badPath, misBoard, misAlgo, loadDllA, loadDllB, algoInitA, algoInitB;
	badPath = misBoard = misAlgo = loadDllA = loadDllB  = algoInitA = algoInitB = true;
	std::string path, pathLoadFailedA, pathLoadFailedB, pathInitFailedA, pathInitFailedB;
	std::string fileNameA, fileNameB, fullFileNameA, fullFileNameB;
	path = pathInitFailedA = pathInitFailedB = pathLoadFailedA = pathLoadFailedB = fileNameA = fileNameB = fullFileNameA = fullFileNameB = "";
	
	HANDLE dir;
	WIN32_FIND_DATAA fileData;
	HINSTANCE hDll;
	std::vector<std::string> dllNamesVec;

	// set path according to argv
	if(argc == 1) //TODO: support prints to console
	{
		path = ".";
	}
	else if(argc == 2)
	{
		path = argv[1];
	}
	else //In case more that 1 argument was given - we choose to stop the program
	{
		throw std::exception("Program takes at most 1 argument!");
	}

	// iterate over files in path
	if (isDirectory(path))
	{
		badPath = false;
		_inputFolder = path;

		// test for empty directory
		std::string s = path + "\\*";

		dir = FindFirstFileA(s.c_str(), &fileData);
		if (dir != INVALID_HANDLE_VALUE)
		{
			// test each file suffix and set variables as needed
			do
			{
				std::string fileName(fileData.cFileName);
				std::string fullFileName = path + "\\" + fileName;

				// In case there are multiple possibilities - we choose to take the last one
				if (endsWith(fileName, ".sboard"))
				{
					_boardFilePath = fullFileName;
					misBoard = false;
				}
				else if (endsWith(fileName, ".dll"))
				{
					dllNamesVec.push_back(fileName);
				}

			} while (FindNextFileA(dir, &fileData));
		}

		/*Sort dll names and take only first 2*/
		std::sort(dllNamesVec.begin(), dllNamesVec.end());

		//TODO: complete!!!
		// Print all errors relevant by order

		//TODO: take from vector
		// = fileData.cFileName;
		//fullFileNameA = path + "\\" + fileName;
		//fullFileNameB = path + "\\" + fileName;
		
		// Load dynamic library
		hDll = LoadLibraryA(fullFileNameA.c_str()); // Notice: Unicode compatible version of LoadLibrary
		if(!hDll)
		{
			loadDllA = false;
			algoInitA = false;
		}

		// Init players (algorithms)
		_playerA;// = new PlayerFile(); //TODO: generalize
		_playerB;// = new PlayerFile(); //TODO: generalize
		
		// Get function pointer
		getShapeFunc = (GetShapeFuncType)GetProcAddress(hDll, "GetShape");
		if(!getShapeFunc)
		{
			std::cout << "could not load function GetShape()" << std::endl;
			return EXIT_FAILURE;
		}



	}	

	/*Validate input by an exact order*/
	if (badPath || misBoard || misAlgo || loadDllA || loadDllB || algoInitA || algoInitB)
	{
		if (badPath)
		{
			std::cout << "Wrong path: " << path.c_str() << std::endl;
		}
		if (misBoard)
		{
			std::cout << "Missing board file (*.sboard) looking in path: " << path.c_str() << std::endl;
		}
		if (misAlgo)
		{
			std::cout << "Missing an algorithm (dll) file looking in path: " << path.c_str() << std::endl;
		}
		if (loadDllA)
		{
			std::cout << "Cannot load dll: " << fullFileNameA << std::endl;
		}
		if (loadDllB)
		{
			std::cout << "Cannot load dll: " << "" << std::endl;
		}
		if (algoInitA)
		{
			std::cout << "Algorithm initialization failed for dll: " << "" << std::endl;
		}
		if (algoInitB)
		{
			std::cout << "Algorithm initialization failed for dll: " << "" << std::endl;
		}
		return false;
	}
	return true; //TODO: implement with FindFirstFileA()
}

bool GameMaker::SetAndValidateBoards()
{
	bool wrongSizeA, wrongSizeB, fewA, fewB, manyA, manyB, adjacent;
	GameBoard fullBoard(_boardFilePath);
	GameBoard copyBoard(fullBoard);

	// shipsData = pair<int, set<char>>
	auto shipsDataA = copyBoard.analyseShips(PLAYER_A);
	auto shipsDataB = copyBoard.analyseShips(PLAYER_B);

	// find out if there are ships with wrong size on the board
	auto illegalShipsA = shipsDataA.second;
	auto illegalShipsB = shipsDataB.second;
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

	/*Validate input by an exact order*/
	if (wrongSizeA || wrongSizeB || fewA || fewB || manyA || manyB || adjacent)
	{
		if (wrongSizeA)
		{
			//print a line for each wrong size type
			for (char shipType : illegalShipsA)
			{
				std::cout << "Wrong size or shape for ship " << shipType << " for player A" << std::endl;
			}
		}
		if (wrongSizeB)
		{
			//print a line for each wrong size type
			for (char shipType : illegalShipsB)
			{
				std::cout << "Wrong size or shape for ship " << shipType << " for player B" << std::endl;
			}
		}
		if (manyA)
		{
			std::cout << "Too many ships for player A" << std::endl;
		}
		if (fewA)
		{
			std::cout << "Too few ships for player A" << std::endl;
		}
		if (manyB)
		{
			std::cout << "Too many ships for player B" << std::endl;
		}
		if (fewB)
		{
			std::cout << "Too few ships for player B" << std::endl;
		}
		if (adjacent)
		{
			std::cout << "Adjacent Ships on Board" << std::endl;
		}
		return false;
	}

	// set local board
	char** rawBoard = fullBoard.getBoard();
	_board = GameBoard(rawBoard, fullBoard.rows(), fullBoard.cols());
	GameBoard::deleteRawBoard(rawBoard, fullBoard.rows(), fullBoard.cols());

	return true;
}
