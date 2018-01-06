#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
class Logger {
public:
	static void openFile();
	static void printInfo(const std::string& message);
	static void printWarning(const std::string& message);
	static void printError(const std::string& message);
	static void closeFile();

private:
	static std::string getTime();
	static std::ofstream myfile;
	static std::chrono::system_clock::time_point start;
};