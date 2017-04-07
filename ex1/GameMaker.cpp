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

	/*Set local Boards & Validate them*/
	if(!SetAndValidateBoards())
	{
		throw GameException("");
	}
	
	/*Init players (algorithms)*/
	_playerA = Player();
	_playerB = Player();
	
	/*Set boards for both players*/
	char** rawBoardA = _boardA.getBoard();
	char** rawBoardB = _boardB.getBoard();
	_playerA.setBoard(const_cast<const char**>(rawBoardA), _boardA.rows(), _boardA.cols());
	_playerB.setBoard(const_cast<const char**>(rawBoardB), _boardB.rows(), _boardB.cols());
	GameBoard::deleteRawBoard(rawBoardA, _boardA.rows(), _boardA.cols());
	GameBoard::deleteRawBoard(rawBoardB, _boardB.rows(), _boardB.cols());
	
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
		*currentPlayerMovesRemaining = (attackPosition != ATTACK_END);
		auto attackResult = opponentBoard->attack(attackPosition);
		int row = attackPosition.first, col = attackPosition.second;

		// notify the players
		_playerA.notifyOnAttackResult(currentPlayerDef, row, col, attackResult);
		_playerB.notifyOnAttackResult(currentPlayerDef, row, col, attackResult);

		// upon hit player gets another turn
		if (attackResult == AttackResult::Hit)
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
		opponentShipsCntr = opponentShipsCntr == &remainingShipsA ? &remainingShipsB : &remainingShipsA;
	}

	// print end game results
	if (remainingShipsA == 0 || remainingShipsB == 0)
		std::cout << "Player " << (remainingShipsA == 0 ? 'B' : 'A') << " won" << std::endl;
	std::cout << "Points:" << std::endl;
	std::cout << "Player A: " << _boardB.getScore() << std::endl;
	std::cout << "Player B: " << _boardA.getScore() << std::endl;
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
		throw std::exception("Program takes at most 1 argument!");
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

	// set local boards
	char** rawBoardA = fullBoard.getBoardForPlayer(PLAYER_A);
	char** rawBoardB = fullBoard.getBoardForPlayer(PLAYER_B);
	_boardA = GameBoard(rawBoardA, fullBoard.rows(), fullBoard.cols());
	_boardB = GameBoard(rawBoardB, fullBoard.rows(), fullBoard.cols());
	GameBoard::deleteRawBoard(rawBoardA, fullBoard.rows(), fullBoard.cols());
	GameBoard::deleteRawBoard(rawBoardB, fullBoard.rows(), fullBoard.cols());

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
		std::string err("Failed to open file in path: ");
		err += movesFilePath;
		throw std::exception(err.c_str());
	}

	// parse the file line by line
	while (std::getline(fin, line))
	{
		std::stringstream lineStream(line);

		// try parsing assuming legal line
		try
		{
			// skip leading spaces
			skipSpaces(lineStream);

			// read first parameter
			lineStream >> row; //TODO: make sure it reads the whole number and not single digits

			// skip spaces and comma between parameters
			skipSpaces(lineStream);
			if(lineStream.peek() != ',') // illegal line - skip
			{
				continue;
			}
			lineStream.ignore();
			skipSpaces(lineStream);

			// read second parameter
			lineStream >> col; //TODO: make sure it reads the whole number and not single digits

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

std::stringstream& GameMaker::skipSpaces(std::stringstream& s)
{
	while (s.peek() == ' ')
	{
		s.ignore();
	}
	return s;
}
