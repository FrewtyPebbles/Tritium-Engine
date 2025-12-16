#pragma once
// This class manages a single log IO "pipe"
#include <fstream>
#include <iostream>
#include <string>
#include <chrono>

using std::string;
using std::chrono::system_clock;

namespace Log {
	enum Domain {
		PHYSICS = 0,
		USER = 1,
		RENDERING = 2
	};

	enum Severity {
		INFO = 0,
		WARNING = 1,
		ERROR = 2,
		FATAL = 3,
		VERBOSE = 4
	};
}
// There should only be one pipe per file.
class LogPipe {
public:
	LogPipe(string output_path, string name);
	~LogPipe();
	void log(string message, Log::Domain domain, Log::Severity severity, std::tm time);

	string name;
private:
	void writeln(string msg);
	static string format(string message, Log::Domain domain, Log::Severity severity, std::tm time);

	std::fstream file;
};