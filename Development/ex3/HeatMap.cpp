#include "HeatMap.h"
#include <map>
#include <unordered_map>

HeatMap::HeatMap(const GameBoard& opponentBoard, const std::vector<int>& shipsLengthsVector)
	: _rows(opponentBoard.rows())
	, _cols(opponentBoard.cols())
	, _depth(opponentBoard.depth())
	, _shipsLengthsVector(shipsLengthsVector)
	, _heatMap(heatCube(_rows * _cols * _depth, -1))
	, _opponentBoard(opponentBoard)
	, _allLegalCoords(opponentBoard.GetAllLegalCoords())
{}

/*returns the coordinate with the highest possible ships positioning options
*@Pre: can only be called once on a HeatMap object
*@Pre: the opponent board is up to date (i.e. after board deduction)
*/
Coordinate HeatMap::hottestCoordinate()
{
	updateHeatMap();
	int maxHeat = -1;
	Coordinate retCoord(1, 1, 1);
	for (int row = 1; row <= _rows; row++)
	{
		for (int col = 1; col <= _cols; col++)
		{
			for (int depth = 1; depth <= _depth; depth++)
			{
				if (maxHeat < (*this)(row, col, depth))
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


/*returns the amount of possibilities to locate a ship on the given coordinate on the opponent board*/
int HeatMap::countPossibleShipsForCoordinate(const Coordinate& c, bool extendHit) const
{
	//std::vector<std::unordered_set<Coordinate>> checkedShipsCoordsSetList;
	int possibleShipsForCoordinate = 0;
	if (!isLegalCoord(c))
	{
		return possibleShipsForCoordinate;
	}

	//preprocessing - counting same ships' lengths
	std::unordered_map<int, int> uniqueShipLengthsWithCount;
	for(int length : _shipsLengthsVector)
	{
		if(uniqueShipLengthsWithCount.count(length) != 0)
		{
			uniqueShipLengthsWithCount[length]++;
		}
		else
		{
			uniqueShipLengthsWithCount[length] = 1;
		}
	}

	for(auto& shipSizeAndCount : uniqueShipLengthsWithCount)
	{
		int shipSize = shipSizeAndCount.first;
		int shipCount = shipSizeAndCount.second;

		//if ship size is 1 no possible shifts.
		// c is definately a legal coordinate
		if(shipSize == 1)
		{
			// if trying to extend hit - can't have ship of size one adjacent to a hit
			if (!extendHit)
			{
				possibleShipsForCoordinate += shipCount;
			}
			continue;
		}

		std::vector<Coordinate> directions{ Coordinate(1, 0, 0), Coordinate(0, 1, 0), Coordinate(0, 0, 1) };

		// For each dimention (orientation)
		for(auto& dir : directions)
		{
			// Create a set of all starting positions in that orientation
			std::unordered_set<Coordinate> startPosShiftSet;
			Coordinate incCoord = c;
			for(int i = 0; i < shipSize; i++, incCoord -= dir)
			{
				startPosShiftSet.insert(incCoord);
			}

			// For each position of a ship
			Coordinate inc(0, 0, 0);
			for(int i = 0; i < shipSize; i++)
			{
				// Check all ships' coordinates legality - with a shift of 'inc'
				std::unordered_set<Coordinate> shipSetToAdd;
				for(auto tempCoord : startPosShiftSet)
				{
					tempCoord += inc;
					if(!isLegalCoord(tempCoord) && (!extendHit || _opponentBoard(tempCoord) != HIT))
					{
						// if extending hit - allow adding hit coords as well
						break;
					}
					shipSetToAdd.insert(tempCoord);
				}
				inc += dir;

				// If legal ship - add to count
				if(shipSetToAdd.size() == shipSize && (!extendHit || IsHitInSet(shipSetToAdd)))
				{
					possibleShipsForCoordinate += shipCount;
				}
			}
		}
	}

	return possibleShipsForCoordinate;
}

bool HeatMap::IsHitInSet(const std::unordered_set<Coordinate>& coords) const
{
	for (auto& coord : coords)
	{
		if (_opponentBoard(coord) == HIT)
		{
			return true;
		}
	}
	return false;
}
