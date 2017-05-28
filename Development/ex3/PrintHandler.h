#pragma once

#include "GameBoard.h"
#include "GameMaker.h"
#include "TournamentMaker.h"
#include <windows.h>
#include <algorithm>

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
#define ATTACK_COLOR_A	(PLAYER_A_COLOR | FG_INT)
#define ATTACK_COLOR_B	(PLAYER_B_COLOR | FG_INT)
#define HIT_COLOR_A		(PLAYER_B_COLOR)
#define HIT_COLOR_B		(PLAYER_A_COLOR)
#define MISS_COLOR		(EMPTY_COLOR)

class PrintHandler
{
public:
	static void init(bool printEnabled, int delayMS)
	{
		_printEnabled = printEnabled;
		_delayMS = delayMS;
		hideCursor();
	}

	static void cleanOutput();
	//static void printInitialBoard(const GameBoard& board);
	//static void printAttackResult(std::pair<int, int> attackPosition, AttackResult attackResult, char attackedPiece, int player);

	static void delay(const int millis = _delayMS / 2)
	{
		if (_printEnabled)
			Sleep(millis);
	}

	static void PrintTournametStage(const std::vector<GameResult>& vector);
	static void PrintWinner(const AlgoData& algoData);
	static void PrintSingleGameWinner(const GameResult& result);
	static void PrintHouseStandings(const std::vector<std::pair<std::string, HouseEntry>>& standings);

private:
	static bool _printEnabled;
	static int _delayMS;

	static void gotoxy(const int x, const int y);
	static void hideCursor();
	static void setTextColor(const WORD color);
	static WORD getTextColor();
};
