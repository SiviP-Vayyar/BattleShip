#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#endif

#include "GameException.h"
#include "GameMaker.h"
#include <iostream>

void print_error(const std::string& s)
{
	std::cout << "Error: " << s.c_str() << std::endl;
}

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Memory leak detection
#endif
	try
	{
		GameMaker gameMaker(argc, argv);
		if (!gameMaker.isInitSuccess())
		{
			return -1;
		}
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
