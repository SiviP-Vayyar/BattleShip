#include "PrintHandler.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

void PrintHandler::cleanOutput()
{
	// evil way
	system("cls");
}

void PrintHandler::PrintWinner(const AlgoData& algoData)
{
	auto winnerLine = "####    Winner Is : " + algoData.name + "    ####";

	std::cout << std::endl;
	std::cout << std::setfill('#') << std::setw(winnerLine.size()) << '#' << std::endl;
	std::cout << winnerLine << std::endl;
	std::cout << std::setfill('#') << std::setw(winnerLine.size()) << '#' << std::endl;
}

void PrintHandler::PrintHouseStandings(const std::vector<std::pair<std::string, HouseEntry>>& standings)
{
	size_t countWidth = standings.size() + 2;
	size_t nameWidth = std::max_element(
		standings.cbegin(), standings.cend(), 
		[](const std::pair<std::string, HouseEntry>& lhs, const std::pair<std::string, HouseEntry>& rhs) -> bool {
			return lhs.first.size() < rhs.first.size();
		}
	)->first.size() + 2;

	size_t digits = static_cast<size_t>(log10(static_cast<double>(PLAYING_ROUNDS * 2))) + 1;
	size_t balanceWidth = digits + 2;

	size_t percentWidth = 6;

	size_t maxPointPerGame = 500; // TODO: deduce an upper bound
	digits = static_cast<size_t>(log10(static_cast<double>(maxPointPerGame * PLAYING_ROUNDS * 2))) + 1;
	size_t scoreWidth = digits + 2;
	
	// adjust sizes in case smaller than header
	nameWidth = max(nameWidth, strlen("Team Name") + 2);
	balanceWidth = max(balanceWidth, strlen("Losses") + 2);
	scoreWidth = max(scoreWidth, strlen("Pts Against") + 2);

	// print header
	std::cout	<< std::left << std::setw(countWidth)	<< "#"
				<< std::left << std::setw(nameWidth)	<< "Team Name"
				<< std::left << std::setw(balanceWidth)	<< "Wins"
				<< std::left << std::setw(balanceWidth)	<< "Losses"
				<< std::left << std::setw(percentWidth)	<< "%"
				<< std::left << std::setw(scoreWidth)	<< "Pts For"
				<< std::left << std::setw(scoreWidth)	<< "Pts Against"
				<< std::left << std::endl << std::endl;

	size_t place = 1;
	for (auto& line : standings)
	{
		double precentage = static_cast<double>(line.second.wins) / (line.second.wins + line.second.losses);
		std::cout	<< std::left << std::setw(countWidth)	<< std::to_string(place++) + '.'
					<< std::left << std::setw(nameWidth)	<< line.first
					<< std::left << std::setw(balanceWidth)	<< line.second.wins
					<< std::left << std::setw(balanceWidth)	<< line.second.losses
					<< std::left << std::setw(percentWidth)	<< std::setprecision(percentWidth - 2) << precentage * 100
					<< std::left << std::setw(scoreWidth)	<< line.second.ptsFor
					<< std::left << std::setw(scoreWidth)	<< line.second.ptsAgainst
					<< std::left << std::endl;
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
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}
