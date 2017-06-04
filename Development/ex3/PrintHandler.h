#pragma once

#include "GameBoard.h"
#include "TournamentMaker.h"
#include <windows.h>
#include <algorithm>

#define PRINT_ENABLED_DEFAULT	true

class PrintHandler
{
public:
	static void init(bool printEnabled)
	{
		_printEnabled = printEnabled;
		hideCursor();
	}

	static void cleanOutput();

	static void PrintWinner(const AlgoData& algoData);
	static void PrintSingleGameWinner(const GameResult& result);
	static void PrintHouseStandings(const std::vector<std::pair<std::string, HouseEntry>>& standings);
	static void PrintHouseStandings(const std::map<std::string, HouseEntry>& standings);

private:
	static bool _printEnabled;

	static void gotoxy(SHORT x, SHORT y);
	static void hideCursor();
	static void setTextColor(const WORD color);
	static WORD getTextColor();
};
