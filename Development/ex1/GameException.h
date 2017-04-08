#pragma once
#include <string>

class GameException : public std::exception
{
public:
	explicit GameException(const std::string& message) : _msg(message)
	{}

	const char* what() const throw () override
	{
		return _msg.c_str();
	}

private:
	std::string _msg;
};
