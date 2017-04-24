#include "GameUtils.h"
#include <algorithm>
#include <sstream>
#include <windows.h>

std::stringstream& GameUtils::skipSpaces(std::stringstream& s)
{
	while(s.peek() == ' ')
	{
		s.ignore();
	}
	return s;
}

/*chek if the path is a valid directory*/
bool GameUtils::isDirectory(const std::string& path)
{
	DWORD ftyp = GetFileAttributesA(path.c_str());

	// test for invalid path
	if(ftyp == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	// test if path is a directory
	if(ftyp & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}
	return false;
}

/*test if str ends with suffix*/
bool GameUtils::endsWith(std::string const & str, std::string const & suffix)
{
	if(suffix.size() > str.size())
	{
		return false;
	}

	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

std::vector<std::string> GameUtils::GetAllFilesSorted(std::string path, std::string endsWith)
{
	HANDLE dir;
	WIN32_FIND_DATAA fileData;
	std::vector<std::string> allFiles;

	// iterate over files in path
	if(isDirectory(path))
	{
		// test for empty directory
		std::string s = path + "\\*" + endsWith;

		dir = FindFirstFileA(s.c_str(), &fileData);
		if(dir != INVALID_HANDLE_VALUE)
		{
			do
			{
				allFiles.push_back(fileData.cFileName);
			} while(FindNextFileA(dir, &fileData));
		}

		/*Sort dll names and take only first 2*/
		std::sort(allFiles.begin(), allFiles.end());
		FindClose(dir);
	}
	return allFiles;
}

void GameUtils::printRawBoard(GameBoard &board) {
	printf("\nprinting board\n");
	for (int row = 1; row <= board.rows(); row++) {
		for (int col = 1; col <= board.cols(); col++) {
			printf("%c", board(row, col));
		}
		printf("|\n");
	}
	printf("done printing board\n");
}