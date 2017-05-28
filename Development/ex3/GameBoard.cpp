#include "GameBoard.h"
#include "GameUtils.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include "TournamentMaker.h"

const char GameBoard::_shipTypes[] = {RUBBER, MISSILE, SUB, DESTROYER};
const int GameBoard::_shipLengths[] = {RUBBER_LEN, MISSILE_LEN, SUB_LEN, DESTROYER_LEN};
const int GameBoard::_shipScores[] = {RUBBER_SCORE, MISSILE_SCORE, SUB_SCORE, DESTROYER_SCORE};

GameBoard::GameBoard(const std::string& path) : GameBoard()
{
	GameBoard& thisBoard = *this;

	//start reading from file
	std::ifstream inputFileStream(path);
	std::string line;

	std::getline(inputFileStream, line); // read dimentions
	std::tie(_rows, _cols, _depth) = GetDimentionsFromLine(line);

	_board = std::move(newEmptyRawBoard(_rows, _cols, _depth));

	for (int d = 1; d <= _depth; d++)
	{
		std::getline(inputFileStream, line); // skip delimiting blank line
		for (int r = 1; r <= _rows; r++)
		{
			std::getline(inputFileStream, line); // read row
			for(int c = 1; c <= _cols; c++) // read cols in a row
			{
				char pieceChar = line[c - 1];
				thisBoard(r, c, d) = isShip(pieceChar) ? pieceChar : EMPTY;
			}
		}
	}
	//TODO: guard access violation for rows and cols

	inputFileStream.close();
}


cube GameBoard::newEmptyRawBoard(int rows, int cols, int depth)
{
	return cube(rows * cols * depth, ' ');
}

void GameBoard::setBoard(const cube& board, int rows, int cols, int depth)
{
	_rows = rows;
	_cols = cols;
	_depth = depth;
	_isSet = true;
	_board = board;
}

/*return a copy of the board*/
cube GameBoard::getBoard() const
{
	return _board;
}

/*return a new raw board with only the player's ships*/
cube GameBoard::getBoardForPlayer(int player) const
{
	cube board(numel());

	for (int i = 0; i < numel(); i++)
	{
		char piece = _board[i];
		board[i] = (playerShipType(player, piece) == piece ? piece : EMPTY);
	}
	
	return board;
}

/*update the board to reflect an attack, and notify on the result and what was hit*/
std::pair<AttackResult, char> GameBoard::boardAttack(const Coordinate& attackPosition)
{
	GameBoard& thisBoard = *this;

	// ignore ATTACK_END
	if (attackPosition == ATTACK_END)
		return std::make_pair(AttackResult::Miss, EMPTY);

	int row = attackPosition.row, col = attackPosition.col, depth = attackPosition.depth;
	char piece = thisBoard(row, col, depth);

	if (isShip(piece))
	{
		// mark hit on the board
		thisBoard(row, col, depth) = SHIP_HIT;

		// determine if just hit or sink, and add to score if needed
		if (isShipSunk(row, col, depth))
		{
			return std::make_pair(AttackResult::Sink, piece);
		}
		return std::make_pair(AttackResult::Hit, piece);
	}
	return std::make_pair(AttackResult::Miss, piece);
}

bool GameBoard::isShip(char piece)
{
	std::vector<char> shipTypes = {RUBBER, MISSILE, SUB, DESTROYER};
	for (char shipType : shipTypes)
	{
		if (playerShipType(PLAYER_A, shipType) == piece ||
			playerShipType(PLAYER_B, shipType) == piece)
		{
			return true;
		}
	}
	return false;
}

char GameBoard::playerShipType(int player, char typeDef)
{
	return char(player == PLAYER_A ? toupper(typeDef) : tolower(typeDef));
}

// given a position on a ship, explore all directions to determine if sunk
/*@pre: assume (row,col,depth) is some position on a ship*/
bool GameBoard::isShipSunk(int row, int col, int depth) //TODO: implement
{
	GameBoard& thisBoard = *this;
	auto increments = {std::pair<int, int>(1, 0), std::pair<int, int>(-1, 0), std::pair<int, int>(0, 1), std::pair<int, int>(0, -1)};

	// go in all 4 directions from starting position and try to find a piece on the ship that is not hit
	for (auto increment : increments)
	{
		int rowInc = increment.first, colInc = increment.second;
		for (int r = row, c = col; isInBoard(r, c); r += rowInc , c += colInc)
		{
			char piece = thisBoard(r, c);

			// if this piece was hit - continue searching
			if (piece == SHIP_HIT)
			{
				continue;
			}
			// if this piece is not part of the ship - stop searching in this direction
			if (!isShip(piece))
			{
				break;
			}
			// if we reached this point, this piece of the ship is intact
			return false;
		}
	}
	// seaerched in all directions for a piece of the ship that was not yet hit - and failed
	return true;
}

/*returns the width and length of the ship in position (row,col)*/
/*@Post: May return an underestimate of the dims, but when covering the entire ship, we will get at
 *least one the will catch an illegal ship */
std::pair<int, int> GameBoard::getShipDimensions(const std::set<Coordinate>& coords) const //TODO: implelemnt
{
	int rowMin, rowMax, colMin, colMax;
	rowMin = colMin = _cols + 1;
	rowMax = colMax = 0;

	for (auto pos : coords)
	{
		rowMin = min(rowMin, pos.first);
		colMin = min(colMin, pos.second);
		rowMax = max(rowMax, pos.first);
		colMax = max(colMax, pos.second);
	}

	return std::make_pair(rowMax - rowMin + 1, colMax - colMin + 1);
}

/*recursively gather all coordinates of the given ship*/
void GameBoard::getShipCoordinates(int row, int col, int depth, std::set<Coordinate>& coords) const //TODO: implelemnt
{
	const GameBoard& thisBoard = *this;

	// if this position is already in the set - stop recursion
	if (coords.find(std::make_pair(row, col)) != coords.end())
	{
		return;
	}

	//insert coordinate to set
	coords.insert(std::make_pair(row, col));

	//continue to neighbors
	for (auto neighbor : getAdjacentCoordinatesAsVector(row,, col))
	{
		// if same type - continue recursion
		int r = neighbor.first;
		int c = neighbor.second;
		if (thisBoard(r, c) == thisBoard(row, col))
		{
			getShipCoordinates(r, c,, coords);
		}
	}
}

/*returns: legalShipsCount, illegalShips*/
std::pair<int, std::set<char>> GameBoard::analyseShips(int player) //TODO: implelemnt
{
	GameBoard& thisBoard = *this;
	int legalShipsCount = 0;
	std::set<char> illegalShips;

	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			// skips pieces that are not this player's ship type
			char piece = thisBoard(row, col);
			if (!isShip(piece) || playerShipType(player, piece) != piece)
			{
				continue;
			}

			std::set<std::pair<int, int>> coords;
			getShipCoordinates(row, col,, coords);
			auto dim = getShipDimensions(coords);
			int size = getShipLength(piece);

			// a legal ship must be of the right size and shape ("narrow")
			if (size != dim.first * dim.second || (dim.first != 1 && dim.second != 1))
			{
				illegalShips.insert(piece);
			}
			// else - a legal ship
			else
			{
				legalShipsCount++;
			}

			ClearShipFromBoard(coords);
		}
	}
	return std::make_pair(legalShipsCount, illegalShips);
}

/*@return: false iff there are no adjacent ships on the board*/
bool GameBoard::isAdjacent() const //TODO: implelemnt
{
	const GameBoard& thisBoard = *this;

	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			char center = thisBoard(row, col);
			if (!isShip(center))
			{
				continue;
			}

			// for every piece on the board that belongs to a ship, check the surrounding pieces
			auto surroudingPositions = getAdjacentCoordinatesAsVector(row,, col);
			for (auto position : surroudingPositions)
			{
				// if the adjacent piece is not a ship - it's good
				char adjacentPiece = thisBoard(position.first, position.second);
				if (!isShip(adjacentPiece))
				{
					continue;
				}

				// if center and adjacentPiece are of different ships or players - it's bad
				if (center != adjacentPiece)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void GameBoard::ClearShipFromBoard(const std::set<Coordinate>& coords)
{
	for(auto pos : coords)
	{
		(*this)(pos.row, pos.col, pos.depth) = EMPTY;
	}

}

int GameBoard::getShipScore(char piece)
{
	for (int i = 0; i < _numShipTypes; i++)
	{
		auto shipType = _shipTypes[i];
		if (playerShipType(PLAYER_A, shipType) == piece ||
			playerShipType(PLAYER_B, shipType) == piece)
		{
			return _shipScores[i];
		}
	}
	return 0;
}

int GameBoard::getShipLength(char piece)
{
	for (int i = 0; i < _numShipTypes; i++)
	{
		auto shipType = _shipTypes[i];
		if (playerShipType(PLAYER_A, shipType) == piece ||
			playerShipType(PLAYER_B, shipType) == piece)
		{
			return _shipLengths[i];
		}
	}
	return 0;
}

std::tuple<int, int, int> GameBoard::GetDimentionsFromLine(const std::string& cs)
{
	//TODO: implement 1X2X4;
	return std::make_tuple(0, 0, 0);
}

char& GameBoard::operator()(int row, int col, int depth)
{
	return _board[((depth - 1)*_cols + (col - 1))*_rows + (row - 1)];
}

char GameBoard::operator()(int row, int col, int depth) const
{
	return _board[((depth - 1)*_cols + (col - 1))*_rows + (row - 1)];
}

GameBoard& GameBoard::operator=(const GameBoard& other)
{
	setBoard(other._board, other._rows, other._cols, other._depth);
	return *this;
}

GameBoard& GameBoard::operator=(GameBoard&& other) noexcept
{
	_board = std::move(other._board); // the move trick
	_rows = other._rows;
	_cols = other._cols;
	_depth = other._depth;
	_isSet = other._isSet;
	
	return *this;
}

/*return a vector of all valid surrounding Coordinates*/
std::vector<Coordinate> GameBoard::getSurroundingCoordinatesAsVector(int row, int col, int depth) const  //TODO: implelemnt
{
	std::vector<std::pair<int, int>> surroundingCoordinates;

	//iterate through all surrounding Coordinates and if valid -> add to return vector
	for (int tempRow = row - 1; tempRow <= row + 1; ++tempRow)
	{
		for (int tempCol = depth - 1; tempCol <= depth + 1; ++tempCol)
		{
			if (tempCol == depth && tempRow == row)
			{
				continue;
			}
			if (isInBoard(tempRow, tempCol))
			{
				surroundingCoordinates.emplace_back(tempRow, tempCol);
			}
		}
	}
	return surroundingCoordinates;
}

/*returns a vector of all adjacent Coordinates (does not include diagonal Coordinates)*/
std::vector<Coordinate> GameBoard::getAdjacentCoordinatesAsVector(int row, int col, int depth) const //TODO: implelemnt
{
	std::vector<std::pair<int, int>> adjacentCoordinates;

	auto increments = {std::pair<int, int>(1, 0), std::pair<int, int>(-1, 0), std::pair<int, int>(0, 1), std::pair<int, int>(0, -1)};
	for (auto inc : increments)
	{
		int r = row + inc.first, c = depth + inc.second;
		if (isInBoard(r, c))
		{
			adjacentCoordinates.emplace_back(r, c);
		}
	}
	return adjacentCoordinates;
}

/*returns a vector of all diagonal Coordinates (does not include diagonal Coordinates)*/
std::vector<Coordinate> GameBoard::getDiagonalCoordinatesAsVector(int row, int col, int depth) const //TODO: implelemnt
{
	std::vector<std::pair<int, int>> diagonalCoordinates;

	auto increments = {std::pair<int, int>(1, 1), std::pair<int, int>(1, -1), std::pair<int, int>(-1, 1), std::pair<int, int>(-1, -1)};
	for (auto inc : increments)
	{
		int r = row + inc.first, c = col + inc.second;
		if (isInBoard(r, c))
		{
			diagonalCoordinates.emplace_back(r, c);
		}
	}
	return diagonalCoordinates;
}

/*
 * Returns the Max Score on board possible for player - meaning it is other player ships' score
 * @Pre: Assuming correct board
 */
int GameBoard::GetMaxScore(int player) const //TODO: implelemnt
{
	GameBoard copyBoard(*this);
	int score = 0;

	// Iterate board
	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			// find enemy ships
			char shipType = copyBoard(row, col);
			if(!isPlayerShip(player, shipType) && !(shipType == EMPTY))
			{
				std::set<std::pair<int, int>> coords;
				copyBoard.getShipCoordinates(row, col,, coords);
				score += getShipScore(shipType);
				copyBoard.ClearShipFromBoard(coords);
			}
		}
	}
	return score;
}
