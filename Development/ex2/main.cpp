#include "GameMaker.h"
#include "GameException.h"
#include <iostream>

void print_error(const std::string& s)
{
	std::cout << "Error: " << s.c_str() << std::endl;
}

int main(int argc, char* argv[])
{
	try
	{
		GameMaker gameMaker(argc, argv);
		gameMaker.RunGame();
	}
	catch (GameException ex)
	{
		/*If a game exception was thrown,
		 *the throwing method already printed the error msgs
		 * all we need is to end the program
		 */
		return -1;
	}
	catch (std::exception ex)
	{
		// For every undefined error
		print_error(ex.what());
		return -1;
	}
	return 0;
}
