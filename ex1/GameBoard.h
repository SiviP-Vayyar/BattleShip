#pragma once

#include "IBattleshipGameAlgo.h"
#include <cctype>
#include "Player.h"

/*ship types definitions*/
#define RUBBER		'B'
#define MISSILE		'P'
#define SUB			'M'
#define DESTROYER	'D'

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
	explicit GameBoard(const std::string& path);
	~GameBoard();

	void setBoard(const char** board, int numRows, int numCols);
	int rows() const { return _rows; }
	int cols() const { return _cols; }
	bool isSet() const { return _isSet; }
	const char** getBoard();
	const char** getBoardForPlayer(int player);
	AttackResult attack(std::pair<int, int> attackPosition);
	int getScore() const { return _score; } /*calculate how well the opponent scored on this board at the end of the game*/
	inline bool isInBoard(int row, int col) const;
	bool isShipSunk(int row, int col);
	void getIllegalShips(int player, std::vector<char>& illegalShips) const;
	int countShips(int player) const;
	bool isAdjacent() const;

	static int getShipScore(char piece);
	static bool isShip(char piece);
	static char playerShipType(int player, char typeDef) { return char(player == PLAYER_A ? toupper(typeDef) : tolower(typeDef)); }

	/*Using 1-based matrix call on vector*/
	char& operator()(int row, int col) const;
	
private:
	char** _board;
	int _rows;
	int _cols;
	bool _isSet;
	int _score = 0;
};
