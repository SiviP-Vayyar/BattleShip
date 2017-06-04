#include "PrintHandler.h"

#include <iostream>
#include <iomanip>

bool PrintHandler::_printEnabled;

void PrintHandler::cleanOutput()
{
	if (!_printEnabled)
		return;

	// cool way
	std::cout << "\x1B[2J\x1B[H";

}

void PrintHandler::PrintWinner(const AlgoData& algoData)
{
	//TODO: implement print
	std::cout << std::endl;
	std::cout << "###########################################" << std::endl;
	std::cout << "####    Winner Is: " << algoData.name << " #####" << std::endl;
	std::cout << "###########################################" << std::endl;
}

void PrintHandler::PrintSingleGameWinner(const GameResult& result)
{
	if(!_printEnabled) // TODO: not necesarrily how we want to print!
		return;
	// print end game results
	cleanOutput();
	int winner = result.Winner();
	if (winner == PLAYER_A || winner == PLAYER_B)
	{
		std::cout << "Player " << (winner == PLAYER_B ? 'B' : 'A') << " won" << std::endl;
	}
	
	std::cout << "Points:" << std::endl;
	std::cout << "Player A: " << result.scoreA << std::endl;
	std::cout << "Player B: " << result.scoreB << std::endl;
}

void PrintHandler::PrintHouseStandings(const std::vector<std::pair<std::string, HouseEntry>>& standings)
{
	std::cout << "#\tTeam Name\t\tWins\tLosses\t%\tPts For\tPts Against" << std::endl << std::endl; //TODO: use iomanip
	size_t place = 1;
	for (auto& line : standings)
	{
		double precentage = static_cast<double>(line.second.wins) / (line.second.wins + line.second.losses);
		std::cout << place++ << ".\t" << line.first << "\t\t" << line.second.wins << "\t" << line.second.losses << "\t" << std::setprecision(4) << precentage*100  << "\t" << line.second.ptsFor << "\t" << line.second.ptsAgainst << "\t" << std::endl;
	}
	std::cout << std::endl;
}

void PrintHandler::PrintHouseStandings(const std::map<std::string, HouseEntry>& standings)
{
	PrintHouseStandings(std::vector<std::pair<std::string, HouseEntry>>(standings.begin(), standings.end()));
}

void PrintHandler::gotoxy(SHORT x, SHORT y)
{
	static HANDLE h = nullptr;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = {x, y};
	SetConsoleCursorPosition(h, c);
}

void PrintHandler::hideCursor()
{
	if (!_printEnabled)
		return;

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

void PrintHandler::setTextColor(const WORD color)
{
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hstdout, color);
}

WORD PrintHandler::getTextColor()
{
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(hstdout, &csbi);
	return csbi.wAttributes;
}
