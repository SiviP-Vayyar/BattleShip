#include "PrintHandler.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>

void PrintHandler::cleanOutput()
{
	// evil way
	system("cls");
}

void PrintHandler::PrintHouseStandings(const std::vector<HouseEntry>& standings)
{
	size_t countWidth = standings.size() + 2;
	size_t nameWidth = std::max_element(
		standings.cbegin(), standings.cend(), 
		[](const HouseEntry& lhs, const HouseEntry& rhs) -> bool {
			return lhs.name.size() < rhs.name.size();
		}
	)->name.size() + 2;

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
				<< std::left << "Elapsed Time"
				<< std::left << std::endl << std::endl;

	size_t place = 1;
	for (auto& line : standings)
	{
		double precentage = static_cast<double>(line.wins) / (line.wins + line.losses);
		std::cout	<< std::left << std::setw(countWidth)	<< std::to_string(place++) + '.'
					<< std::left << std::setw(nameWidth)	<< line.name
					<< std::left << std::setw(balanceWidth)	<< line.wins
					<< std::left << std::setw(balanceWidth)	<< line.losses
					<< std::left << std::setw(percentWidth)	<< std::setprecision(percentWidth - 2) << precentage * 100
					<< std::left << std::setw(scoreWidth)	<< line.ptsFor
					<< std::left << std::setw(scoreWidth)	<< line.ptsAgainst
					<< std::left << line.time
					<< std::left << std::endl;
	}
	std::cout << std::endl;
}

void PrintHandler::PrintHouseStandings(const std::map<std::string, HouseEntry>& m)
{
	std::vector<HouseEntry> vec;
	vec.reserve(m.size());
	for (auto& val : m)
	{
		vec.push_back(val.second);
	}
	std::sort(vec.begin(), vec.end(), HouseEntry::Compare());
	PrintHouseStandings(vec);
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
