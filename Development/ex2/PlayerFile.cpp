#include "PlayerFile.h"
#include <sstream>
#include <fstream>
#include "GameMaker.h"


void PlayerFile::SetMoves(std::vector<std::pair<int, int>> moves)
{
	_myMoves = moves;
	_movesIterator = _myMoves.begin();
}

std::pair<int, int> PlayerFile::attack()
{
	if (_player == PLAYER_NOT_YET_KNOWN)
	{
		throw std::exception("attack() was called before setBoard was called!");
	}

	/*Return next move, and advance the iterator*/
	if (_movesIterator != _myMoves.end())
	{
		return *_movesIterator++;
	}
	return ATTACK_END;
}

bool PlayerFile::init(const std::string& path) //TODO: SIVAN
{
	_algoMovesFolder = path;
	std::string attackFilePath = "";//getSortedFiles(path, endsWith = null);
	//TODO: utils for all static funcs of files & stuff
	// Set algorithm moves for both players
	//TODO: get file according to sorted attack files

	SetMoves(getMovesFromFile(_algoMovesFolder));

	return true;
}

/*@pre: assume opponent's board was loaded and set. required for GameBoard::isInBoard method*/
std::vector<std::pair<int, int>> PlayerFile::getMovesFromFile(const std::string& movesFilePath) const
{
	std::vector<std::pair<int, int>> moves;
	std::ifstream fin(movesFilePath);
	std::string line;
	int row, col;

	// check if file failed to open
	if(!fin)
	{
		std::string err("Failed to open file in path: ");
		err += movesFilePath;
		throw std::exception(err.c_str());
	}

	// parse the file line by line
	while(std::getline(fin, line))
	{
		std::stringstream lineStream(line);

		// try parsing assuming legal line
		try
		{
			// skip leading spaces
			skipSpaces(lineStream);

			// read first parameter
			lineStream >> row;

			// skip spaces and comma between parameters
			skipSpaces(lineStream);
			if(lineStream.peek() != ',') // illegal line - skip
			{
				continue;
			}
			lineStream.ignore();
			skipSpaces(lineStream);

			// read second parameter
			lineStream >> col;

			// verify legal position
			if(!_myBoard.isInBoard(row, col))
			{
				continue;
			}

			// add values to vector
			moves.push_back(std::pair<int, int>(row, col));
		}

		// if line has illegal format - skip
		catch(std::exception e)
		{
		}
	}

	fin.close();
	return moves;
}

std::stringstream& PlayerFile::skipSpaces(std::stringstream& s)
{
	while(s.peek() == ' ')
	{
		s.ignore();
	}
	return s;
}


IBattleshipGameAlgo* GetAlgorithm()
{
	_instancesVec.push_back(new PlayerFile());			// Create new instance and keep it in vector
	return _instancesVec[_instancesVec.size() - 1];		// Return last instance
}
