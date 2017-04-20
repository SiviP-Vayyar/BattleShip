#pragma once

#include "GameBoard.h"
#include <windows.h>

#define DELAY_TURNS	2
#define DELAY_ATTACK	1

#define PRINT_MISS		' '
#define PRINT_ATTACK	'@'
#define PRINT_HIT		'*'

// TODO: can we just make the whole class static?
class PrintHandler
{
public:
	static void init(bool printEnabled) { _printEnabled = printEnabled; hideCursor(); }
	static void printInitialBoard(const GameBoard& board);
	static void printAttackResult(const std::pair<int, int> attackPosition, const AttackResult attackResult);
	static void delay(const int seconds = DELAY_TURNS) { Sleep(seconds * 1000);  }
	
private:
	static bool _printEnabled;

	static void gotoxy(const int x, const int y);
	static void hideCursor();
};
