#include "GameMaker.h"
#include "GameException.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <tchar.h>
//#include <stdio.h>
#include <strsafe.h>
#include <winapifamily.h>

GameMaker::GameMaker(int argc, char* argv[])
{
	/*Validate input & Set input arguments*/
	std::string inputPath;
	if(!ParseInput(argc, argv, inputPath))
	{
		throw GameException("");
	}

	/*Validate Boards*/
	if(!ValidateBoards())
	{
		throw GameException("");
	}
	
	/*Init players (algorithms)*/
	_playerA = Player();
	_playerB = Player();
	
	/*Set boards for both players*/
	_playerA.setBoard(_boardA.getBoard(), _boardA.rows(), _boardA.cols());
	_playerB.setBoard(_boardB.getBoard(), _boardB.rows(), _boardB.cols());
	
	/*Set algorithm moves for both players*/
	_playerA.SetMoves(getMovesFromFile(_attackFilePathA,_boardB));
	_playerB.SetMoves(getMovesFromFile(_attackFilePathB,_boardA));
}

GameMaker::~GameMaker()
{
}

/*@pre: assume players and board were set and validated*/
void GameMaker::RunGame()
{
	// counters and flags
	int remainingShipsA = MAX_SHIPS, remainingShipsB = MAX_SHIPS;
	bool movesRemainingA = true, movesRemainingB = true;

	// alternating variables for game loop
	auto currentPlayerDef = PLAYER_A;
	auto currentPlayer = &_playerA;
	auto currentPlayerMovesRemaining = &movesRemainingA;
	auto opponentBoard = &_boardB;
	auto opponentShipsCntr = &remainingShipsB;

	// game loop
	while (remainingShipsA > 0 && remainingShipsB > 0 && (movesRemainingA || movesRemainingB))
	{
		// get attack from player and play it on the board
		auto attackPosition = currentPlayer->attack();
		*currentPlayerMovesRemaining = (attackPosition != ATTACK_END); //TODO: need to address that situation - only other player continues attacking!
		auto attackResult = opponentBoard->attack(attackPosition);
		int row = attackPosition.first, col = attackPosition.second;

		// notify the players
		_playerA.notifyOnAttackResult(currentPlayerDef, row, col, attackResult);
		_playerB.notifyOnAttackResult(currentPlayerDef, row, col, attackResult);

		// upon hit player gets another turn
		if(attackResult == AttackResult::Hit)
		{
			continue;
		}
		// upon sink player gets another turn + update ships counter
		if (attackResult == AttackResult::Sink)
		{
			*opponentShipsCntr = *opponentShipsCntr - 1;
			continue;
		}

		// switch players for next round
		currentPlayerDef = currentPlayerDef == PLAYER_A ? PLAYER_B : PLAYER_A;
		currentPlayer = currentPlayer == &_playerA ? &_playerB : &_playerA;
		currentPlayerMovesRemaining = currentPlayerMovesRemaining == &movesRemainingA ? &movesRemainingB : &movesRemainingA;
		opponentBoard = opponentBoard == &_boardA ? &_boardB : &_boardA;
		opponentShipsCntr = ((opponentShipsCntr == &remainingShipsA) ? &remainingShipsB : &remainingShipsA);
	}

	// print end game results
	if (remainingShipsA == 0 || remainingShipsB == 0)
		std::cout << "Player " << (remainingShipsA == 0 ? 'B' : 'A') << " won" << std::endl;
	std::cout << "Points:" << std::endl;
	std::cout << "Player A: " << _boardB.getScore() << std::endl;
	std::cout << "Player B: " << _boardA.getScore() << std::endl;
	//TODO: third print case: No win - print only scores.
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
bool GameMaker::ParseInput(int argc, char* argv[], std::string& path)
{
	bool badPath, misBoard, misFileA, misFileB;
	badPath = misBoard = misFileA = misFileB = true;

	// set path according to argv
	if(argc == 1)
	{
		path = ".";
	}
	else if(argc == 2)
	{
		path = argv[1];
	}
	else //In case more that 1 argument was given - we choose to stop the program
	{
		throw GameException("Program takes only 1 argument!");
	}

	// iterate over files in path
	if (isDirectory(path))
	{
		HANDLE dir;
		WIN32_FIND_DATA fileData;
		badPath = false;
		_inputFolder = path;

		// test for empty directory
		std::string s = path + "\\*";
		std::wstring wpath(s.begin(), s.end());
		dir = FindFirstFile(wpath.c_str(), &fileData);
		if (dir != INVALID_HANDLE_VALUE)
		{
			// test each file suffix and set variables as needed
			do
			{
				std::wstring wfileName = fileData.cFileName;
				std::string fileName(wfileName.begin(), wfileName.end());
				std::string fullFileName = path + "\\" + fileName;

				// In case there are multiple possibilities - we choose to take the last one
				if (endsWith(fileName, ".sboard"))
				{
					_boardFilePath = fullFileName;
					misBoard = false;
				}
				else if (endsWith(fileName, ".attack-a"))
				{
					_attackFilePathA = fullFileName;
					misFileA = false;
				}
				else if (endsWith(fileName, ".attack-b"))
				{
					_attackFilePathB = fullFileName;
					misFileB = false;
				}

			} while (FindNextFile(dir, &fileData));
		}
	}	

	/*Validate input by an exact order*/
	if(badPath || misBoard || misFileA || misFileB)
	{
		std::cout << "Wrong path: " << path.c_str() << std::endl;
		
		if(badPath)
		{
			return false;
		}
		if(misBoard)
		{
			std::cout << "Missing board file (*.sboard) looking in path: " << path.c_str() << std::endl;
		}
		if(misFileA)
		{
			std::cout << "Missing attack file for player A (*.attack-a) looking in path: " << path.c_str() << std::endl;
		}
		if(misFileB)
		{
			std::cout << "Missing attack file for player B (*.attack-b) looking in path: " << path.c_str() << std::endl;
		}
		return false;
	}

	return true;
}

bool GameMaker::ValidateBoards() //Noam
{
	bool fewA, fewB, manyA, manyB, adjacent;
	fewA = fewB = manyA = manyB = adjacent = false;

	/*Validate input by an exact order*/
	//use local variables to read files - if we get here we can assume the files are there.
	//Too many ships for player A
	//Too few ships for player A
	//Too many ships for player B
	//Too few ships for player B
	//Adjacent Ships on Board


	if(fewA || fewB || manyA || manyB || adjacent)
	{
		//print "Wrong Path: <path>"
		if(fewA)
		{
			//print
		}
		if(fewB)
		{
			//print
		}
		if(manyA)
		{
			//print
		}
		if(manyB)
		{
			//print
		}
		if(adjacent)
		{
			//print
		}
		return false;
	}


	//TODO: Set local boards

	return true;
}

/*@pre: assume opponent's board was loaded and set. required for GameBoard::isInBoard method*/
std::vector<std::pair<int, int>> GameMaker::getMovesFromFile(
	const std::string& movesFilePath, const GameBoard& opponentBoard) const
{
	std::vector<std::pair<int, int>> moves;
	std::ifstream fin(movesFilePath);
	std::string line;
	int row, col;

	// check if file failed to open
	if(!fin)
	{
		throw GameException("Failed to open file in path: " + movesFilePath);
	}

	// parse the file line by line
	while (std::getline(fin, line))
	{
		std::stringstream lineStream(line);

		// try parsing assuming legal line
		try
		{
			// skip leading spaces
			while (lineStream.peek() == ' ') //TODO: a line cannot start with spaces - change to illegal
				lineStream.ignore();

			// read first parameter
			lineStream >> row; //TODO: make sure can read any number - even 2 digits (maybe 3 even - is that a problem?)
			//TODO: also make sure it is a digit and not a char with the value "7" or something 

			// skip spaces and comma between parameters
			while(lineStream.peek() == ' ')
			{
				lineStream.ignore();
			}
			if(lineStream.peek() != ',') // illegal line - skip
			{
				continue;
			}
			lineStream.ignore();
			while(lineStream.peek() == ' ')
			{
				lineStream.ignore();
			}

			// read second parameter
			lineStream >> col; //TODO: same goes here - maybe do it in isInBoard() ?

			// verify legal position
			if(!opponentBoard.isInBoard(row, col))
			{
				continue;
			}

			// add values to vector
			moves.push_back(std::pair<int, int>(row, col));
		}

		// if line has illegal format - skip
		catch (std::exception e)
		{
			continue;
		}
	}

	fin.close();
	return moves;
}
