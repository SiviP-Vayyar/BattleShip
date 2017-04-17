
#include <sstream>


inline std::stringstream& skipSpaces(std::stringstream& s)
{
	while(s.peek() == ' ')
	{
		s.ignore();
	}
	return s;
}
