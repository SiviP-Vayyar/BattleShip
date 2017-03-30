#pragma once
#include <exception>
#include <string>

class GameException : public std::exception
{
public:
	explicit GameException(const std::string& message) : _msg(message)
	{}

	/** Returns a pointer to the (constant) error description.
	*  @return A pointer to a const char*. The underlying memory
	*          is in posession of the Exception object. Callers must
	*          not attempt to free the memory.
	*/
	const char* what() const throw () override
	{
		return _msg.c_str();
	}

private:
	std::string _msg;
};
