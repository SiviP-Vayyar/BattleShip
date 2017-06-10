#pragma once

#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"

typedef std::vector<int> heatCube;
class HeatMap
{
public:
	HeatMap(const GameBoard& opponentBoard, const std::vector<int>& shipsLengthsVector);
	~HeatMap() = default;	
	HeatMap(const HeatMap& map) = delete;
	HeatMap() = delete;
	
	Coordinate hottestCoordinate();
	int countPossibleShipsForCoordinate(const Coordinate &coord) const;

private:
	void updateHeatMap();
	bool isLegalCoord(const Coordinate& c) const { return _allLegalCoords.count(c) != 0; }

	int& operator()(int row, int col, int depth) { return _heatMap[((depth - 1)*_cols + (col - 1))*_rows + (row - 1)]; } // used as getter, e.g. char piece = board(1,2,3)
	int& operator()(const Coordinate& c) { return (*this)(c.row, c.col, c.depth); }


	int _rows;
	int _cols;
	int _depth;
	const std::vector<int>& _shipsLengthsVector;
	heatCube _heatMap;
	const GameBoard& _opponentBoard;
	const std::unordered_set<Coordinate> _allLegalCoords;
};