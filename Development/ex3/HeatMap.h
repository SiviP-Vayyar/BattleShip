#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"

typedef std::vector<int> heatCube;
class HeatMap
{
public:
	HeatMap(GameBoard myBoard, GameBoard opponentBoard);
	~HeatMap() = default;	

	/* When I want to make a move on opponent's board*/
	Coordinate hottestCoordinate();
	

private:
	int countPossibleShipsForCoordinate(Coordinate coord);
	void updateHeatMap();
	std::vector<std::unordered_set<Coordinate>> getUnCheckedCoordsSetListForCoord(Coordinate coord);

	int& operator()(int row, int col, int depth) { return _heatMap[((depth - 1)*_cols + (col - 1))*_rows + (row - 1)]; } // used as getter, e.g. char piece = board(1,2,3)
	int& operator()(const Coordinate& c) { return (*this)(c.row, c.col, c.depth); }


	int _rows;
	int _cols;
	int _depth;
	std::vector<int> _shipsLengthsVector;
	heatCube _heatMap;
	GameBoard _myBoard;
	GameBoard _opponentBoard;
};