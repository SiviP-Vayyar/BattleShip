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

/*board dimentions for ex1*/
#define BOARD_ROWS		10
#define BOARD_COLS		10

/*Player Macros*/
#define PLAYER_NOT_YET_KNOWN	-1
#define PLAYER_A				0
#define PLAYER_B				1
#define ATTACK_END				std::pair<int, int>(-1, -1) // To mark the end of the moves list

/*Board Macros*/
#define HIT						'H'
#define MISS 					'O'
#define SINK 					'S'
#define MAX_SHIPS 				5

class GameBoard
{
public:
	friend class PlayerBase;
	GameBoard() : _board(nullptr), _rows(0), _cols(0), _isSet(false) {}
	explicit GameBoard(const std::string& path); //c'tor from file
	explicit GameBoard(const char* const* board, int numRows, int numCols) : GameBoard() { setBoard(board, numRows, numCols); }
	
	GameBoard(const GameBoard& other) : GameBoard(other._board, other._rows, other._cols) {} // copy c'tor
	GameBoard(GameBoard&& other) noexcept : _board(nullptr), _rows(other._rows), _cols(other._cols), _isSet(other._isSet) { std::swap(_board, other._board); } // move c'tor
	GameBoard& operator=(const GameBoard& other); // copy assignment
	GameBoard& operator=(GameBoard&& other) noexcept; // move assignment
	~GameBoard(); // d'tor

	/*Using 1-based matrix call on vector*/

	char operator()(int row, int col) const; // used as getter, e.g. char piece = board(1,2)
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
	std::vector<std::pair<int, int>> getDiagonalCoordinatesAsVector(int row, int col) const;
	int GetMaxScore(int player) const { return 1; }; //TODO: for player A get max score of player B's ships

	/*Board static methods*/
	static int getShipScore(char piece);
	static int getShipLength(char piece);
	static bool isShip(char piece);
	static char playerShipType(int player, char piece);
	static char** newEmptyRawBoard(int rows, int cols);
	static void deleteRawBoard(char** board, int rows, int cols);

	/* check if ship belongs to this player*/
	static bool isPlayerShip(int player, char shipType) { return (shipType != EMPTY && playerShipType(player, shipType) == shipType); }

	struct BoardErrors
	{
		BoardErrors() {	wrongSizeA = wrongSizeB = fewA = fewB = manyA = manyB = adjacent = true; }
		bool wrongSizeA, wrongSizeB, fewA, fewB, manyA, manyB, adjacent;
		operator bool() const { return wrongSizeA || wrongSizeB || fewA || fewB || manyA || manyB || adjacent; }
	};

private:
	char** _board;
	int _rows;
	int _cols;
	bool _isSet;

	static const int _numShipTypes = 4;
	static const char _shipTypes[];
	static const int _shipLengths[];
	static const int _shipScores[];

	void setBoard(const char* const* board, int numRows, int numCols);
	char& operator()(int row, int col); // used as setter, e.g. board(1,2) = 'M'
};
