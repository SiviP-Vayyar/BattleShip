#include "PrintHandler.h"

#include <iostream>

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

void PrintHandler::printInitialBoard(const GameBoard& board)
{
	if (!_printEnabled)
		return;

	WORD savedColor = getTextColor();

	for (int row = 1; row <= board.rows(); row++)
	{
		for (int col = 1; col <= board.cols(); col++)
		{
			char piece = board(row, col);
			gotoxy(col, row);

			if (piece == EMPTY)
				setTextColor(EMPTY_COLOR);
			else if (GameBoard::playerShipType(PLAYER_A, piece) == piece)
				setTextColor(PLAYER_A_COLOR);
			else
				setTextColor(PLAYER_B_COLOR);

			putchar(board(row, col));
		}
	}

	gotoxy(1, board.rows() + 2);
	setTextColor(PLAYER_A_COLOR);
	std::cout << "Pl.A" << std::endl;

	gotoxy(board.cols() - 3, board.rows() + 2);
	setTextColor(PLAYER_B_COLOR);
	std::cout << "Pl.B" << std::endl;

	setTextColor(savedColor);
}

void PrintHandler::printAttackResult(std::pair<int, int> attackPosition, AttackResult attackResult, char attackedPiece, int player)
{
	if (!_printEnabled)
		return;

	if (attackPosition == ATTACK_END)
		return;

	int row = attackPosition.first, col = attackPosition.second;
	WORD savedColor = getTextColor();

	gotoxy(col, row);
	setTextColor(player == PLAYER_A ? ATTACK_COLOR_A : ATTACK_COLOR_B);
	putchar(PRINT_ATTACK);
	delay();

	gotoxy(col, row);
	if (attackResult == AttackResult::Miss && attackedPiece != SHIP_HIT)
	{
		setTextColor(MISS_COLOR);
		putchar(PRINT_MISS);
	}
	else // either hit, or "miss" that is actually a second hit on the same position+
	{
		setTextColor(player == PLAYER_A ? HIT_COLOR_A : HIT_COLOR_B);
		putchar(PRINT_HIT);
	}

	setTextColor(savedColor);
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
