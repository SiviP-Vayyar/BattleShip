#pragma once

#include "IBattleshipGameAlgo.h"
#include <cctype>
#include "Player.h"

/*ship types definitions*/
#define RUBBER		'B'
#define MISSILE		'P'
#define SUB			'M'
#define DESTROYER	'D'
#define EMPTY		' '

/*ship attack definitions*/
#define SHIP_HIT	'X'

/*ship types score*/
#define RUBBER_SCORE	2
#define MISSILE_SCORE	3
#define SUB_SCORE		7
#define DESTROYER_SCORE	8

/*ship types length*/
#define RUBBER_LEN		1
#define MISSILE_LEN		2
#define SUB_LEN			3
#define DESTROYER_LEN	4


class GameBoard
{
public:
	GameBoard() : _board(nullptr), _rows(0), _cols(0), _isSet(false){}
	GameBoard(const char* const* board, int numRows, int numCols) { setBoard(board, numRows, numCols); }
	GameBoard(const GameBoard& other) : GameBoard(other._board, other._rows, other._cols) {} // copy c'tor
	explicit GameBoard(const std::string& path);
	~GameBoard();
	static void deleteRawBoard(char** board, int rows, int cols);

	int rows() const { return _rows; }
	int cols() const { return _cols; }
	bool isSet() const { return _isSet; }
	char** getBoard() const;
	char** getBoardForPlayer(int player) const;
	AttackResult attack(std::pair<int, int> attackPosition);
	int getScore() const { return _score; } /*calculate how well the opponent scored on this board at the end of the game*/
	bool isInBoard(int row, int col) const { return 0 < row && row <= _rows && 0 < col && col <= _cols; }
	bool isShipSunk(int row, int col);
	std::pair<int, int> getShipDimensions(int row, int col) const;
	std::vector<char> getIllegalShips(int player) const;
	int countShips(int player) const;
	bool isAdjacent() const;
	std::vector<std::pair<int, int>> getSurroundingCoordinatesAsVector(int row, int col) const;
	std::vector<std::pair<int, int>> getAdjacentCoordinatesAsVector(int row, int col) const;

	static int getShipScore(char piece);
	static int getShipLength(char piece);
	static bool isShip(char piece);
	static char playerShipType(int player, char typeDef) { return char(player == PLAYER_A ? toupper(typeDef) : tolower(typeDef)); }

	/*Using 1-based matrix call on vector*/
	char& operator()(int row, int col) const;
	GameBoard& operator=(const GameBoard& other) { setBoard(other._board, other._rows, other._cols); return *this; };
	
private:
	char** _board;
	int _rows;
	int _cols;
	bool _isSet;
	int _score = 0;

	void setBoard(const char* const* board, int numRows, int numCols);

};
