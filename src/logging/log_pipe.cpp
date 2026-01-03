#include "Engine/logging/log_pipe.h"
#include <sstream>
#include <filesystem>

LogPipe::LogPipe(std::string output_path, string name) {
	this->name = name;
	{// Ensure the file exists
		namespace fs = std::filesystem;

		fs::path path(output_path);

		// Create parent directories if needed
		if (path.has_parent_path()) {
			fs::create_directories(path.parent_path());
		}

		std::ofstream touch(output_path, std::ios::out | std::ios::trunc);
		if (!touch.is_open()) {
			throw std::runtime_error("Failed to create log file: " + output_path);
		}
		touch.close();
	}

	this->file = std::fstream(output_path, std::ios::in | std::ios::out | std::ios::app);

	if (!this->file.is_open()) {
		throw std::runtime_error("Failed to open log file: " + output_path);
	}
}

LogPipe::~LogPipe() {
	if (this->file.is_open())
		this->file.close();
}

string LogPipe::format(string message, Log::Domain domain, Log::Severity severity, std::tm time) {
	string domain_string;

	switch (domain) {
	
	case Log::Domain::PHYSICS:
		domain_string = "PHYSICS";
		break;
	
	case Log::Domain::RENDERING:
		domain_string = "RENDERING";
		break;
	
	case Log::Domain::USER:
		domain_string = "USER";
		break;
	}

	string severity_string;

	switch (severity) {
	
	case Log::Severity::INFO:
		severity_string = "INFO";
		break;
	
	case Log::Severity::WARNING:
		severity_string = "WARNING";
		break;

	case Log::Severity::ERROR:
		severity_string = "ERROR";
		break;

	case Log::Severity::FATAL:
		severity_string = "FATAL ERROR";
		break;

	case Log::Severity::VERBOSE:
		severity_string = "VERBOSE";
		break;
	case Log::Severity::DEBUG:
		severity_string = "DEBUG";
		break;
	}

	std::ostringstream ss;
	ss << "[" << std::put_time(&time, "%Y-%m-%d %H:%M:%S") << "] "
		<< "[" << domain_string << "] "
		<< "[" << severity_string << "] "
		<< message;

	return ss.str();
}

void LogPipe::writeln(string msg) {
	this->file << msg << "\n";
	this->file.flush();
}

void LogPipe::log(string message, Log::Domain domain, Log::Severity severity, std::tm time) {
	writeln(format(message, domain, severity, time));
}