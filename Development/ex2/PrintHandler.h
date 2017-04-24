#pragma once

#include "GameBoard.h"
#include <windows.h>

#define PRINT_ENABLED_DEFAULT	true
#define PRINT_DELAY_DEFAULT		500

#define PRINT_MISS		' '
#define PRINT_ATTACK	'@'
#define PRINT_HIT		'*'

#define FG_BLU 0x01
#define FG_GRN 0x02
#define FG_RED 0x04
#define FG_INT 0x08
#define BG_BLU 0x10
#define BG_GRN 0x20
#define BG_RED 0x40
#define BG_INT 0x80

#define PLAYER_A_COLOR	(BG_INT | FG_RED | FG_BLU)
#define PLAYER_B_COLOR	(BG_INT | FG_GRN)
#define EMPTY_COLOR		(BG_BLU)
#define ATTACK_COLOR	(BG_INT | FG_RED | FG_INT)
#define HIT_COLOR		(ATTACK_COLOR ^ FG_INT)
#define MISS_COLOR		(EMPTY_COLOR)

// TODO: can we just make the whole class static?
class PrintHandler
{
public:
	static void init(bool printEnabled, int delayMS) { _printEnabled = printEnabled; _delayMS = delayMS; hideCursor(); }
	static void cleanOutput();
	static void printInitialBoard(const GameBoard& board);
	static void printAttackResult(const std::pair<int, int> attackPosition, const AttackResult attackResult, const char attackedPiece);
	static void delay(const int millis = _delayMS / 2) { if (_printEnabled) Sleep(millis);  }
	
private:
	static bool _printEnabled;
	static int _delayMS;

	static void gotoxy(const int x, const int y);
	static void hideCursor();
	static void setTextColor(const WORD color);
	static WORD getTextColor();
};
