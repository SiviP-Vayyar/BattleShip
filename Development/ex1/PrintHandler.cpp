#include "PrintHandler.h"

void PrintHandler::printInitialBoard(const GameBoard& board)
{
	for (int row = 1; row <= board.rows(); row++)
	{
		for (int col = 1; col <= board.cols(); col++)
		{
			gotoxy(col, row);
			putchar(board(row, col));
		}
	}
}

void PrintHandler::printAttackResult(const std::pair<int, int> attackPosition, const AttackResult attackResult)
{
	int row = attackPosition.first, col = attackPosition.second;
	gotoxy(col, row);
	putchar('@');
	delay(DELAY_ATTACK);
	gotoxy(col, row);
	if (attackResult == AttackResult::Miss)
	{
		putchar(' ');
	}
	else
	{
		putchar('*');
	}
}

void PrintHandler::gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h, c);
}