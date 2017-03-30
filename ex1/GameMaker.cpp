#include "GameMaker.h"
#include "GameException.h"
#include <iostream>


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
	_playerA.SetMoves(getMovesFromFile(_attackFilePathA));
	_playerB.SetMoves(getMovesFromFile(_attackFilePathB));
}

GameMaker::~GameMaker()
{
}

/*@pre: assume players and board were set and validated*/
void GameMaker::RunGame() //ZOHAR
{
	auto currentPlayerDef = PLAYER_A;
	auto currentPlayer = &_playerA;
	auto currentBoard = &_boardB;
	while (true)
	{
		// get attack from player and play it on the board
		auto attackPosition = currentPlayer->attack();
		auto attackResult = currentBoard->attack(attackPosition);
		int row = attackPosition.first, col = attackPosition.second;

		// notify the players
		_playerA.notifyOnAttackResult(currentPlayerDef, row, col, attackResult);
		_playerB.notifyOnAttackResult(currentPlayerDef, row, col, attackResult);

		// determine if the game is finished
		// TODO

		// switch players for next round
		currentPlayerDef = currentPlayerDef == PLAYER_A ? PLAYER_B : PLAYER_A;
		currentPlayer = currentPlayer == &_playerA ? &_playerB : &_playerA;
		currentBoard = currentBoard == &_boardA ? &_boardB : &_boardA;
	}
}

/*Validate input, parse it, and set all needed local variables*/
bool GameMaker::ParseInput(int argc, char* argv[], std::string& path) //NOAM
{
	bool badPath, misBoard, misFileA, misFileB;
	badPath = misBoard = misFileA = misFileB = false;

	/*Validate input by an exact order*/
	// TODO: calculate booleans
	// Path don't exist / bad format
	// Missing board file
	// Missing attack file for player A
	// Missing attack file for player B

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

	//TODO: set local variables
	/*
	* 	std::string _inputFolder;
	*std::string _boardFilePath;
	*std::string _attackFilePathA;
	*std::string _attackFilePathB;
	*/

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

std::vector<std::pair<int, int>> GameMaker::getMovesFromFile(const std::string& movesFilePath) //NOAM
{
	//TODO: implement
	return std::vector<std::pair<int, int>>();
}
