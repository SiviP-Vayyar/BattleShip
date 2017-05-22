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

/*Player Macros*/
#define PLAYER_NOT_YET_KNOWN	-1
#define PLAYER_A				0
#define PLAYER_B				1
#define ATTACK_END				Coordinate(-1, -1, -1) // To mark the end of the moves list

/*Board Macros*/
#define HIT						'H'
#define MISS 					'O'
#define SINK 					'S'
#define MAX_SHIPS 				5

typedef std::vector<char> cube;

class GameBoard
{
public:
	friend class PlayerBase;
	GameBoard() : _rows(0), _cols(0), _depth(0), _isSet(false) {}
	explicit GameBoard(const std::string& path); //c'tor from file
	explicit GameBoard(const cube& board, int rows, int cols, int depth) : GameBoard() { setBoard(board, rows, cols, depth); }
	
	GameBoard(const GameBoard& other) : GameBoard(other._board, other._rows, other._cols, other._depth) {} // copy c'tor
	GameBoard(GameBoard&& other) noexcept : _board(std::move(other._board)), _rows(other._rows), _cols(other._cols), _depth(other._depth), _isSet(other._isSet) {} // move c'tor
	GameBoard& operator=(const GameBoard& other); // copy assignment
	GameBoard& operator=(GameBoard&& other) noexcept; // move assignment
	~GameBoard(); // d'tor

	/*Using 1-based matrix call on vector*/

	char operator()(int row, int col, int depth) const; // used as getter, e.g. char piece = board(1,2,3)
	std::pair<AttackResult, char> boardAttack(const Coordinate& attackPosition);

	/*Board info methods*/
	cube getBoard() const;
	cube getBoardForPlayer(int player) const;
	int rows() const { return _rows; }
	int cols() const { return _cols; }
	int depth() const { return _depth; }
	bool isSet() const { return _isSet; }
	bool isInBoard(int row, int col, int depth) const { return 0 < row && row <= _rows && 0 < col && col <= _cols && 0 < depth && depth <= _depth; }
	bool isShipSunk(int row, int col, int depth);
	bool isAdjacent() const;

	/*Board validation methods*/
	std::pair<int, std::set<char>> analyseShips(int player);
	void ClearShipFromBoard(const std::set<Coordinate>& coords);
	std::pair<int, int> getShipDimensions(const std::set<Coordinate>& coords) const;
	void getShipCoordinates(int row, int col, int depth, std::set<Coordinate>& coords) const;
	std::vector<Coordinate> getSurroundingCoordinatesAsVector(int row, int col, int depth) const;
	std::vector<Coordinate> getAdjacentCoordinatesAsVector(int row, int col, int depth) const;
	std::vector<Coordinate> getDiagonalCoordinatesAsVector(int row, int col, int depth) const;
	int GetMaxScore(int player) const;

	/*Board static methods*/
	static int getShipScore(char piece);
	static int getShipLength(char piece);
	static bool isShip(char piece);
	static char playerShipType(int player, char piece);
	static cube newEmptyRawBoard(int rows, int cols, int depth);

	/* check if ship belongs to this player*/
	static bool isPlayerShip(int player, char shipType) { return (shipType != EMPTY && playerShipType(player, shipType) == shipType); }

	struct BoardErrors
	{
		BoardErrors() {	wrongSizeA = wrongSizeB = fewA = fewB = manyA = manyB = adjacent = true; }
		bool wrongSizeA, wrongSizeB, fewA, fewB, manyA, manyB, adjacent;
		operator bool() const { return wrongSizeA || wrongSizeB || fewA || fewB || manyA || manyB || adjacent; }
	};

private:
	cube _board;
	int _rows;
	int _cols;
	int _depth;
	bool _isSet;

	static const int _numShipTypes = 4;
	static const char _shipTypes[];
	static const int _shipLengths[];
	static const int _shipScores[];
	static std::tuple<int, int, int> GetDimentionsFromLine(const std::string& cs);

	void setBoard(const cube& board, int rows, int cols, int depth);
	int numel() const { return _rows * _cols * _depth; }
	char& operator()(int row, int col, int depth); // used as setter, e.g. board(1,2) = 'M'
};
