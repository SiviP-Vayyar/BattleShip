#pragma once

#include "IBattleshipGameAlgo.h"
#include <set>
#include <vector>

/*ship types definitions*/
#define RUBBER			'B'
#define MISSILE			'P'
#define SUB				'M'
#define DESTROYER		'D'
#define EMPTY			' '

/*ship attack definitions*/
#define SHIP_HIT		'X'

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
	GameBoard() : _board(nullptr), _rows(0), _cols(0), _isSet(false) {}
	GameBoard(const char* const* board, int numRows, int numCols) : GameBoard() { setBoard(board, numRows, numCols); }
	GameBoard(const GameBoard& other) : GameBoard(other._board, other._rows, other._cols) {} // copy c'tor
	explicit GameBoard(const std::string& path);
	~GameBoard();

	/*Using 1-based matrix call on vector*/
	char& operator()(int row, int col);			// used as setter, e.g. board(1,2) = 'M'
	char operator()(int row, int col) const;	// used as getter, e.g. char piece = board(1,2)
	GameBoard& operator=(const GameBoard& other);
	std::pair<AttackResult, char> attack(std::pair<int, int> attackPosition);

	/*Board info methods*/
	char** getBoard() const;
	char** getBoardForPlayer(int player) const;
	int rows() const { return _rows; }
	int cols() const { return _cols; }
	bool isSet() const { return _isSet; }
	bool isInBoard(int row, int col) const { return 0 < row && row <= _rows && 0 < col && col <= _cols; }
	bool isShipSunk(int row, int col);
	bool isAdjacent() const;

	/*Board validation methods*/
	std::pair<int, std::set<char>> analyseShips(int player);
	std::pair<int, int> getShipDimensions(const std::set<std::pair<int, int>>& coords) const;
	void getShipCoordinates(int row, int col, std::set<std::pair<int, int>>& coords) const;
	std::vector<std::pair<int, int>> getSurroundingCoordinatesAsVector(int row, int col) const;
	std::vector<std::pair<int, int>> getAdjacentCoordinatesAsVector(int row, int col) const;

	/*Board static methods*/
	static int getShipScore(char piece);
	static int getShipLength(char piece);
	static bool isShip(char piece);
	static char playerShipType(int player, char piece);
	static void deleteRawBoard(char** board, int rows, int cols);
	
private:
	char** _board;
	int _rows;
	int _cols;
	bool _isSet;

	void setBoard(const char* const* board, int numRows, int numCols);
};
