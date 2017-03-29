#pragma once
#include <map>

class ArgParser
{
public:
	ArgParser();
	~ArgParser();

	void AddArgument(std::string name, std::string defaultValue);
	void Parse(int argc, char* argv[]);
	
	std::string GetArgument(std::string name);

private:
	std::map<std::string, std::string> _args;
};
