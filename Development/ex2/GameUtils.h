#pragma once

#include <vector>

class GameUtils
{
public:
	static std::stringstream& skipSpaces(std::stringstream& s);
	/*chek if the path is a valid directory*/
	static bool isDirectory(const std::string& path);
	/*test if str ends with suffix*/
	static bool endsWith(std::string const & str, std::string const & suffix);
	/*Using for lexicographic precedence*/
	static std::vector<std::string> GetAllFilesSorted(std::string path, std::string endsWith = "");

	/*Used to select an element uniformly at random from a container*/
	template <typename I>
	static I randomElement(I begin, I end);
};