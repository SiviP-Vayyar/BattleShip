#include "HeatMap.h"

HeatMap::HeatMap(GameBoard myBoard, GameBoard opponentBoard, std::vector<int> shipsLengthsVector) :_myBoard(myBoard), _opponentBoard(opponentBoard)
{
	_rows = myBoard.rows();
	_cols = myBoard.cols();
	_depth = myBoard.depth();
	_heatMap = heatCube(_rows * _cols * _depth, -1);
	_shipsLengthsVector = shipsLengthsVector;
}

/*returns the coordinate with the highest possible ships positioning options
*@Pre: can only be called once on a HeatMap object
*@Pre: the opponent board is up to date (i.e. after board deduction)
*/
Coordinate HeatMap::hottestCoordinate()
{
	updateHeatMap();
	int maxHeat = -1;
	Coordinate retCoord = Coordinate(1, 1, 1);
	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			for (int depth = 1; depth <= _depth; depth++)
			{
				if (maxHeat <= (*this)(row, col, depth))
				{
					maxHeat = (*this)(row, col, depth);
					retCoord = Coordinate(row, col, depth);
				}
			}
		}
	}
	return retCoord;
}


/*updates the heat map with the count of how many ships can be placed on each position on the board
*@Pre: all the values in the heatmap are set to -1
*@Pre: the opponent board is up to date
*/
void HeatMap::updateHeatMap()
{
	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			for (int depth = 1; depth <= _depth; depth++)
			{
				(*this)(row, col, depth) = countPossibleShipsForCoordinate(Coordinate(row, col, depth));
			}
		}
	}
}


/*return a vector of Coordinates sets - with each set holding coordinates of a ship that intercects with the given coordinate
*@Post: the sets returned might hold invalid cordinates (i.e. coordinates out of board limits and cordinates of non empty cells on the board)
*/
std::vector<std::unordered_set<Coordinate>> HeatMap::getUnCheckedCoordsSetListForCoord(Coordinate coord) const
{
	std::vector<std::unordered_set<Coordinate>> retVector = std::vector<std::unordered_set<Coordinate>>();
	for (int shipSize : _shipsLengthsVector)
	{
		if (shipSize == 1)//if ship size is 1 no possible shifts
		{
			std::unordered_set<Coordinate> locationCoords = std::unordered_set<Coordinate>();
			locationCoords.insert(coord);
			retVector.push_back(locationCoords);
			continue;
		}
		std::unordered_set<Coordinate> startRowShiftSet = std::unordered_set<Coordinate>();
		std::unordered_set<Coordinate> startColShiftSet = std::unordered_set<Coordinate>();
		std::unordered_set<Coordinate> startDepthShiftSet = std::unordered_set<Coordinate>();
		for (int increment = 0; increment < shipSize; increment++)
		{
			startRowShiftSet.insert(Coordinate(coord.row - increment, coord.col, coord.depth));
			startColShiftSet.insert(Coordinate(coord.row, coord.col - increment, coord.depth));
			startDepthShiftSet.insert(Coordinate(coord.row, coord.col, coord.depth - increment));
		}
		for (int increment = 0; increment < shipSize; increment++)
		{
			std::unordered_set<Coordinate> rowSetToAdd = std::unordered_set<Coordinate>();
			for (auto tempCoord: startRowShiftSet)
			{
				rowSetToAdd.insert(Coordinate(tempCoord.row + increment, tempCoord.col, tempCoord.depth));
			}
			std::unordered_set<Coordinate> colSetToAdd = std::unordered_set<Coordinate>();
			for (auto tempCoord : startColShiftSet)
			{
				colSetToAdd.insert(Coordinate(tempCoord.row, tempCoord.col + increment, tempCoord.depth));
			}
			std::unordered_set<Coordinate> depthSetToAdd = std::unordered_set<Coordinate>();
			for (auto tempCoord : startDepthShiftSet)
			{
				depthSetToAdd.insert(Coordinate(tempCoord.row, tempCoord.col, tempCoord.depth + increment));
			}
			retVector.push_back(rowSetToAdd);
			retVector.push_back(colSetToAdd);
			retVector.push_back(depthSetToAdd);
		}
	}
	return retVector;
}


/*returns the amount of possibilities to locate a ship on the given coordinate on the opponent board*/
int HeatMap::countPossibleShipsForCoordinate(Coordinate coord) const
{
	int possibleShipCount = 0;
	std::vector<std::unordered_set<Coordinate>> unCheckedShipsCoordsSetList = getUnCheckedCoordsSetListForCoord(coord);
	
	for (auto coordSet: unCheckedShipsCoordsSetList)
	{
		bool validCoordsForShip = true;
		for (auto coord: coordSet)
		{
			if (!_opponentBoard.isInBoard(coord))
			{
				validCoordsForShip = false;
				break;
			}
			if (_opponentBoard(coord) != EMPTY)
			{
				validCoordsForShip = false;
				break;
			}
		}
		if (validCoordsForShip)
		{
			possibleShipCount++;
		}
	}
	return possibleShipCount;
}
