#pragma once

#include <ctime>
#include <fstream>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <string>

enum LogLevel
{
	Debug,
	Info,
	Warning,
	Error
};

class Logger
{
public:

	static void Shutdown()
	{
		log("==========", Info);
		log("Log Finish", Info);
		log("==========", Info);
		Instance()._fout.close();
	}

	static void Init(std::string logFile)
	{
		Instance()._fout.open(logFile, std::fstream::out | std::fstream::app);
		log("========", Info);
		log("Log Init", Info);
		log("========", Info);
	}

	static std::string GetTimeStamp()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", timeinfo);
		return std::string(buffer);
	}

	static void log(std::string item, LogLevel level = Info)
	{
		std::lock_guard<std::mutex> guard(Instance()._logMutex);

		std::cout << item << std::endl;

		Instance()._fout	<< std::left << std::setw(22) << GetTimeStamp().c_str()
				<< "(" << std::right << std::setw(8) << std::setfill('0') << std::this_thread::get_id() << std::setfill(' ') << ") "
				<< std::left << std::setw(8)  << ("[" + enum2str(level) + "]: ").c_str()
				<< item.c_str() << std::endl;
	}

	static std::string enum2str(LogLevel level)
	{
		switch(level)
		{
		case LogLevel::Debug:
			return "Debug";
		case LogLevel::Info:
			return "Info";
		case LogLevel::Warning:
			return "Warning";
		case LogLevel::Error:
			return "Error";
		}
		return "Info";
	}
	static Logger& Instance()
	{
		static Logger _instance;
		return _instance;
	}

	Logger(const Logger&) = delete;
	void operator=(const Logger&) = delete;

private:
	Logger(){}
	std::ofstream _fout;
	std::mutex _logMutex;
};