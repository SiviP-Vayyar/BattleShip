#include "GameMaker.h"
#include <iostream>

GameMaker::GameMaker(int argc, char* argv[])
{
	/*Build parse arguments*/
	_parser = ArgParser();
	_parser.AddArgument("--folder...dsfsdfa", "");
	_parser.AddArgument("--help", "false");
	//parser.AddArgument("", "");

	/*Parse command line arguments*/
	_parser.Parse(argc, argv);
	
	/*Set input arguments*/
	//Zohar

	/*Init players (algorithms)*/
	//Noam
	
	/*Set boards for both players*/
	//Sivan
	
	/*Set algorithm moves for both players*/
	//Sivan

}

GameMaker::~GameMaker()
{
}

GameMaker& GameMaker::operator=(const GameMaker& otheBoard)
{
	return *this; //TODO: implement
}

void GameMaker::RunGame()
{
}

void GameMaker::SetConfiguration()
{
}

bool GameMaker::ValidateInput(const std::string& path)
{
	bool badPath, misBoard, misFileA, misFileB;
	badPath = misBoard = misFileA = misFileB = false;

	/*Validate input by an exact order*/
	// TODO: calculate booleans NOAM
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
	return true;
}

bool GameMaker::ValidateBoards() //Noam
{
	bool fewA, fewB, manyA, manyB, adjacent;
	fewA = fewB = manyA = manyB = adjacent = false;

	/*Validate input by an exact order*/
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
	return true;
}