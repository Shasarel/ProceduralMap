#include "Logger.h"
#include <ctime>
std::ofstream Logger::myfile;
std::chrono::system_clock::time_point Logger::start;
void Logger::openFile()
{
	start = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(start);
	std::tm now_tm;
	localtime_s(&now_tm, &t);
	std::string filename = "logs/" + std::to_string(now_tm.tm_year + 1900) + "-" + std::to_string(now_tm.tm_mon + 1) + "-" + std::to_string(now_tm.tm_mday) + "--" + std::to_string(now_tm.tm_hour) + "-" + std::to_string(now_tm.tm_min) + "-" + std::to_string(now_tm.tm_sec) + ".log";

	myfile.open(filename.c_str(), std::ios_base::app);
}

void Logger::printInfo(const std::string& message)
{
	std::string msg = getTime() + "[INFO] " + message;
	myfile << msg << std::endl;
	std::cout << msg << std::endl;
}

void Logger::printWarning(const std::string& message)
{
	std::string msg = getTime() + "[WARNING] " + message;
	myfile << msg << std::endl;
	std::cout << msg << std::endl;
}

void Logger::printError(const std::string& message)
{
	std::string msg = getTime() + "[ERROR] " + message;
	myfile << msg << std::endl;
	std::cout << msg << std::endl;
}

void Logger::closeFile()
{
	myfile.close();
}

std::string Logger::getTime()
{
	auto now = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
	long long miliseconds = elapsed.count();

	long long seconds = miliseconds / 1000;
	miliseconds = miliseconds - seconds * 1000;

	long long minutes = seconds / 60;
	seconds = seconds - minutes * 60;

	long long hours = minutes / 60;
	minutes = minutes - hours * 60;

	std::string time = "[" + std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds) + "." + std::to_string(miliseconds) + "]";

	return time;
}
