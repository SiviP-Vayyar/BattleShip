#include "GameMaker.h"
#include <iostream>

void print_error(const std::string& s)
{
	std::cout << "Error: " << s.c_str() << std::endl;
}

int main(int argc, char* argv[])
{
	try
	{
		GameMaker gameMaker = GameMaker(argc, argv);
		gameMaker.RunGame();
	}
	catch (std::exception e)
	{
		print_error(e.what());
		return -1;
	}
	
	return 0;
}
