#pragma once

#include <string> // Actually not required here - but here due to students request
#include <memory> // for std::uniqe_ptr


/*Notice:
 * Usually it is bad practice to have several classes/structs in the same header.
 * Here we use this practice only for simplicity considerations,
 * since we only want to have 1 common file in this excersice.
 * You should avoid from doing so yourself!
 */

enum class AttackResult
{
	Miss,
	Hit,
	Sink
};

struct Coordinate
{
	const int x, y, z;
	Coordinate(int x1, int y1, int z1) : x(x1), y(y1), z(z1) {}
};

/*This is a wrapper to the Board's Data
 * You should derive from it, make sure you set all protected members in the derived class.
 */
class BoardData
{
public:
	int rows() const { return _rows; }
	int cols() const { return _cols; }
	int depth() const { return _depth; }
	virtual ~BoardData() = default;
	virtual char charAt(Coordinate c) const = 0; //returns only selected players' chars
protected:
	int _rows = 0;
	int _cols = 0;
	int _depth = 0;
};

class IBattleshipGameAlgo
{
public:
	virtual ~IBattleshipGameAlgo() = default;
	virtual void setPlayer(int player) = 0;				// called every time the player changes his order
	virtual void setBoard(const BoardData& board) = 0;	// called once at the beginning of each new game
	virtual Coordinate attack() = 0;					// ask player for his move
	virtual void notifyOnAttackResult(int player, Coordinate move, AttackResult result) = 0; // last move result
};

#ifdef ALGO_EXPORTS									// A flag defined in this project's Preprocessor's Definitions
#define ALGO_API extern "C" __declspec(dllexport)	// If we build - export
#else
#define ALGO_API extern "C" __declspec(dllimport)	// If someone else includes this .h - import
#endif

ALGO_API std::unique_ptr<IBattleshipGameAlgo> GetAlgorithm(); // This method must be implemented in each player(algorithm) .cpp file
