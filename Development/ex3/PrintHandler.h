#pragma once

#include "GameBoard.h"
#include "TournamentMaker.h"
#include <windows.h>

class PrintHandler
{
public:
	static void cleanOutput();
	static void PrintHouseStandings(const std::vector<std::pair<std::string, HouseEntry>>& standings);
	static void PrintHouseStandings(const std::map<std::string, HouseEntry>& standings);

private:
	static void gotoxy(SHORT x, SHORT y);
	static void hideCursor();
};
