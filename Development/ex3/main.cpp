#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#endif

#include "GameException.h"
#include <iostream>
#include "TournamentMaker.h"

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Memory leak detection
#endif
	int retCode = 0;
	try
	{
		TournamentMaker tMaker(argc, argv);
		if (!tMaker.isInitSuccess())
		{
			retCode = -1;
		}
		else
		{
			tMaker.RunTournament();
		}
	}
	catch (GameException ex)
	{
		/* If a game exception was thrown,
		 * the throwing method already printed the error msgs
		 * all we need is to end the program
		 */
		retCode = -1;
	}
	catch (std::exception ex)
	{
		// For every undefined error
		Logger::log(ex.what(), Error);
		retCode = -1;
	}
	
	Logger::Shutdown();
	return retCode;
}
