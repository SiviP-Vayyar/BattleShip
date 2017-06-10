#pragma once

#include <vector>
#include "GameBoard.h"
#include <tuple>

/*Player Macros*/
#define PLAYER_NOT_YET_KNOWN	-1
#define PLAYER_A				0
#define PLAYER_B				1

class GameUtils
{
public:
	static std::stringstream& skipSpaces(std::stringstream& s);
	/*chek if the path is a valid directory*/
	static bool isDirectory(const std::string& path);
	/*test if str ends with suffix*/
	static bool endsWith(std::string const& str, std::string const& suffix);
	/*Using for lexicographic precedence*/
	static std::vector<std::string> GetAllFilesSorted(std::string path, std::string endsWith = "");

	/*Used to select an element uniformly at random from a container*/
	template <typename I>
	static I randomElement(I begin, I end);
	//static void printRawBoard(const GameBoard& board);
	static std::string GetTeamNameFromFileName(const std::string& path);
};

template <typename I>
I GameUtils::randomElement(I begin, I end)
{
	auto n = std::distance(begin, end);
	auto divisor = (RAND_MAX + 1) / n;

	auto k = n;
	do { k = std::rand() / divisor; }
	while (k >= n);

	std::advance(begin, k);
	return begin;
}

inline bool operator==(const Coordinate& c1, const Coordinate& c2)
{
	return c1.row == c2.row && c1.col == c2.col && c1.depth == c2.depth;
}

namespace std
{
	template <> struct hash<Coordinate> //std::hash specialization for Coordinate in order to use unordered_map
	{
		size_t operator()(Coordinate const& c) const
		{
			return static_cast<size_t>(5 * c.row + 17 * c.col + 1023 * c.depth + 89); //TODO: better hash function?
		}
	};
}

