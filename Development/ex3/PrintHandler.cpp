#include "PrintHandler.h"

#include <iostream>
#include <iomanip>

bool PrintHandler::_printEnabled;
int PrintHandler::_delayMS;

void PrintHandler::cleanOutput()
{
	if (!_printEnabled)
		return;
	delay(_delayMS);

	gotoxy(0, 0);
	for (int i = 0; i < 20; i++)
	{
		std::cout << std::string(20, ' ') << std::endl;
	}
	gotoxy(0, 0);
}

//void PrintHandler::printInitialBoard(const GameBoard& board)
//{
//	if (!_printEnabled)
//		return;
//
//	WORD savedColor = getTextColor();
//
//	for (int row = 1; row <= board.rows(); row++)
//	{
//		for (int col = 1; col <= board.cols(); col++)
//		{
//			char piece = board(row, col);
//			gotoxy(col, row);
//
//			if (piece == EMPTY)
//				setTextColor(EMPTY_COLOR);
//			else if (GameBoard::playerShipType(PLAYER_A, piece) == piece)
//				setTextColor(PLAYER_A_COLOR);
//			else
//				setTextColor(PLAYER_B_COLOR);
//
//			putchar(board(row, col));
//		}
//	}
//
//	gotoxy(1, board.rows() + 2);
//	setTextColor(PLAYER_A_COLOR);
//	std::cout << "Pl.A" << std::endl;
//
//	gotoxy(board.cols() - 3, board.rows() + 2);
//	setTextColor(PLAYER_B_COLOR);
//	std::cout << "Pl.B" << std::endl;
//
//	setTextColor(savedColor);
//}

//void PrintHandler::printAttackResult(std::pair<int, int> attackPosition, AttackResult attackResult, char attackedPiece, int player)
//{
//	if (!_printEnabled)
//		return;
//
//	if (attackPosition == ATTACK_END)
//		return;
//
//	int row = attackPosition.first, col = attackPosition.second;
//	WORD savedColor = getTextColor();
//
//	gotoxy(col, row);
//	setTextColor(player == PLAYER_A ? ATTACK_COLOR_A : ATTACK_COLOR_B);
//	putchar(PRINT_ATTACK);
//	delay();
//
//	gotoxy(col, row);
//	if (attackResult == AttackResult::Miss && attackedPiece != SHIP_HIT)
//	{
//		setTextColor(MISS_COLOR);
//		putchar(PRINT_MISS);
//	}
//	else // either hit, or "miss" that is actually a second hit on the same position+
//	{
//		setTextColor(player == PLAYER_A ? HIT_COLOR_A : HIT_COLOR_B);
//		putchar(PRINT_HIT);
//	}
//
//	setTextColor(savedColor);
//}

void PrintHandler::PrintTournametStage(const std::vector<GameResult>& vector)
{
	//TODO: implement print
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
	std::cout << "#\tTeam Name\t\tWins\tLosses\t%\tPts For\tPts Against" << std::endl << std::endl;
	size_t place = 1;
	for (auto& line : standings)
	{
		double precentage = static_cast<double>(line.second.wins) / (line.second.wins + line.second.losses);
		std::cout << place++ << ".\t" << line.first << "\t\t" << line.second.wins << "\t" << line.second.losses << "\t" << std::setprecision(4) << precentage*100  << "\t" << line.second.ptsFor << "\t" << line.second.ptsAgainst << "\t" << std::endl;
	}
	std::cout << std::endl;
}

void PrintHandler::gotoxy(int x, int y)
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
