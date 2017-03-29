#include "GameMaker.h"

GameMaker::GameMaker(int argc, char* argv[])
{
	/*Build parse arguments*/
	_parser = ArgParser();
	_parser.AddArgument("--folder...", "");
	_parser.AddArgument("--help", "false");
	//parser.AddArgument("", "");

	/*Parse command line arguments*/
	_parser.Parse(argc, argv);

	/*Set input arguments*/


	/*Init players (algorithms)*/

	
	/*Set boards for both players*/

	
	/*Set algorithm moves for both players*/


}

bool ValidateInput(const std::string& path)
{
	bool misBoard, misFileA, misFileB;
	misBoard = misFileA = misFileB = false;

	/*Validate input by an exact order*/
	// Missing board file
	// Missing attack file for player A
	// Missing attack file for player B

	if(misBoard || misFileA || misFileB)
	{
		//print "Wrong Path: <path>"
		if(misBoard)
		{
			//print
		}
		if(misFileA)
		{
			//print
		}
		if(misFileB)
		{
			//print
		}
		return false;
	}
	return true;
}

bool ValidateBoards()
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