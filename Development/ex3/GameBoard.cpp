#include "GameBoard.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <tuple>

const char GameBoard::_shipTypes[] = {RUBBER, MISSILE, SUB, DESTROYER};
const int GameBoard::_shipLengths[] = {RUBBER_LEN, MISSILE_LEN, SUB_LEN, DESTROYER_LEN};
const int GameBoard::_shipScores[] = {RUBBER_SCORE, MISSILE_SCORE, SUB_SCORE, DESTROYER_SCORE};

GameBoard::GameBoard(const std::string& path) : GameBoard()
{
	GameBoard& thisBoard = *this;
	_boardName = path;

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

GameBoard::GameBoard(const BoardData& boardData) : _rows(boardData.rows()), _cols(boardData.cols()), _depth(boardData.depth()), _isSet(true)
{
	for(int depth = 1; depth <= _depth; depth++)
	{
		for(int col = 1; col <= _cols; col++)
		{
			for(int row = 1; row <= _rows; row++)
			{
				_board.push_back(boardData.charAt(Coordinate(row, col, depth)));
			}
		}
	}
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
bool GameBoard::isShipSunk(int row, int col, int depth)
{
	GameBoard& thisBoard = *this;
	auto increments =
		{ Coordinate(1, 0, 0), Coordinate(-1, 0, 0),
		Coordinate(0, 1, 0), Coordinate(0, -1, 0),
		Coordinate(0, 0, 1), Coordinate(0, 0, -1) };


	// go in all 4 directions from starting position and try to find a piece on the ship that is not hit
	for (auto increment : increments)
	{
		int rowInc = increment.row, colInc = increment.col, depthInc = increment.depth;
		for (int r = row, c = col, d = depth; isInBoard(r, c, d); r += rowInc , c += colInc, d += depthInc)
		{
			char piece = thisBoard(r, c, d);

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
 *least one the will catch an illegal ship 
 * Abuse of Coordinate to send back 3D dims
 */
Coordinate GameBoard::getShipDimensions(const std::unordered_set<Coordinate>& coords) const //TODO: implelemnt
{
	int rowMin, rowMax, colMin, colMax, depthMin, depthMax;
	rowMin = _rows + 1;
	colMin = _cols + 1;
	depthMin = _depth + 1;
	rowMax = colMax = depthMax = 0;

	for (auto pos : coords)
	{
		rowMin = min(rowMin, pos.row);
		colMin = min(colMin, pos.col);
		depthMin = min(depthMin, pos.depth);
		rowMax = max(rowMax, pos.row);
		colMax = max(colMax, pos.col);
		depthMax = max(depthMax, pos.depth);
	}

	return Coordinate(rowMax - rowMin + 1, colMax - colMin + 1, depthMax - depthMin + 1);
}


/*returns a vector containing all sizes of ships on board*/
std::vector<int> GameBoard::getShipsOnBoardSizes()
{
	std::vector<int> retVector = std::vector<int>();
	std::unordered_set<Coordinate> shipsCoordsSet = std::unordered_set<Coordinate>();
	bool skip;

	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			for (int depth = 1; depth <= _depth; depth++)
			{
				//check if the ship has already been seen
				for (Coordinate coord : shipsCoordsSet) {
					skip = (coord.col == col && coord.row == row && coord.depth == depth) ? true : false;
					if (skip)
						break;
				}
				//skip this coordinate if it was already seen in a ship
				if (skip)
					continue;

				if (isShip((*this)(row, col, depth))) 
				{
					getShipCoordinates(row, col, depth, shipsCoordsSet);
					retVector.push_back(getShipLength((*this)(row, col, depth)));
				}
			}
		}
	}
	return retVector;
}


/*recursively gather all coordinates of the given ship*/
void GameBoard::getShipCoordinates(int row, int col, int depth, std::unordered_set<Coordinate>& coords) const
{
	const GameBoard& thisBoard = *this;
	Coordinate currCoordinate(row, col, depth);

	// if this position is already in the set - stop recursion
	if (coords.find(currCoordinate) != coords.end())
	{
		return;
	}

	//insert coordinate to set
	coords.insert(currCoordinate);

	//continue to neighbors
	for (auto neighbor : getAdjacentCoordinatesAsVector(row, col, depth))
	{
		// if same type - continue recursion
		int r = neighbor.row;
		int c = neighbor.col;
		int d = neighbor.depth;
		if (thisBoard(r, c, d) == thisBoard(row, col, depth))
		{
			getShipCoordinates(r, c, d, coords);
		}
	}
}

/*returns: legalShipsCount, illegalShips*/
std::pair<int, std::set<char>> GameBoard::analyseShips(int player)
{
	GameBoard& thisBoard = *this;
	int legalShipsCount = 0;
	std::set<char> illegalShips;

	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			for(int depth = 1; depth <= _depth; depth++)
			{
				// skips pieces that are not this player's ship type
				char piece = thisBoard(row, col, depth);
				if(!isShip(piece) || playerShipType(player, piece) != piece)
				{
					continue;
				}

				std::unordered_set<Coordinate> coords;
				getShipCoordinates(row, col, depth, coords);
				auto dim = getShipDimensions(coords);
				int size = getShipLength(piece);

				// a legal ship must be of the right size and shape ("narrow")
				bool r1Dim = (dim.row == 1), c1Dim = (dim.col == 1), d1Dim = (dim.depth == 1);
				bool rWiseShip = c1Dim && d1Dim;
				bool cWiseShip = r1Dim && d1Dim;
				bool dWiseShip = r1Dim && c1Dim;

				if((size != dim.row * dim.col * dim.depth) || (rWiseShip || cWiseShip || dWiseShip))
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
	}
	return std::make_pair(legalShipsCount, illegalShips);
}

/*@return: false iff there are no adjacent ships on the board*/
bool GameBoard::isAdjacent() const
{
	const GameBoard& thisBoard = *this;

	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			for(int depth = 1; col <= _depth; depth++)
			{
				char center = thisBoard(row, col, depth);
				if(!isShip(center))
				{
					continue;
				}

				// for every piece on the board that belongs to a ship, check the surrounding pieces
				auto surroudingPositions = getAdjacentCoordinatesAsVector(row, col, depth);
				for(auto position : surroudingPositions)
				{
					// if the adjacent piece is not a ship - it's good
					char adjacentPiece = thisBoard(position.row, position.col, position.depth);
					if(!isShip(adjacentPiece))
					{
						continue;
					}

					// if center and adjacentPiece are of different ships or players - it's bad
					if(center != adjacentPiece)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

void GameBoard::ClearShipFromBoard(const std::unordered_set<Coordinate>& coords)
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
	size_t pos;
	std::string s = cs;
	std::string token;
	std::string delimiter = "x";
	std::vector<int> dims;

	while((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		dims.push_back(stoi(token));
		s.erase(0, pos + delimiter.length());
	}

	if(dims.size() < 3)
	{
		//TODO: not enough dims - do something
	}

	return std::make_tuple(dims[0], dims[1], dims[2]);
}

//GameBoard& GameBoard::operator=(const GameBoard& other)
//{
//	setBoard(other._board, other._rows, other._cols, other._depth);
//	return *this;
//}
//
//GameBoard& GameBoard::operator=(GameBoard&& other) noexcept
//{
//	_board = std::move(other._board); // the move trick
//	_rows = other._rows;
//	_cols = other._cols;
//	_depth = other._depth;
//	_isSet = other._isSet;
//	
//	return *this;
//}

/*returns a vector of all adjacent Coordinates (does not include diagonal Coordinates)*/
std::vector<Coordinate> GameBoard::getAdjacentCoordinatesAsVector(int row, int col, int depth) const
{
	std::vector<Coordinate> adjacentCoordinates;

	auto increments =
		{Coordinate(1, 0, 0), Coordinate(-1, 0, 0),
		Coordinate(0, 1, 0), Coordinate(0, -1, 0),
		Coordinate(0, 0, 1), Coordinate(0, 0, -1) };
	for (auto inc : increments)
	{
		int r = row + inc.row, c = col + inc.col, d = depth + inc.depth;
		if (isInBoard(r, c, d))
		{
			adjacentCoordinates.emplace_back(r, c, d);
		}
	}
	return adjacentCoordinates;
}

/*
 * Returns the Max Score on board possible for player - meaning it is other player ships' score
 * @Pre: Assuming correct board
 */
int GameBoard::GetMaxScore(int player) const
{
	GameBoard copyBoard(*this);
	int score = 0;

	// Iterate board
	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			for(int depth = 1; depth <= _depth; depth++)
			{
				// find enemy ships
				char shipType = copyBoard(row, col, depth);
				if(!isPlayerShip(player, shipType) && !(shipType == EMPTY))
				{
					std::unordered_set<Coordinate> coords;
					copyBoard.getShipCoordinates(row, col, depth, coords);
					score += getShipScore(shipType);
					copyBoard.ClearShipFromBoard(coords);
				}
			}
		}
	}
	return score;
}
