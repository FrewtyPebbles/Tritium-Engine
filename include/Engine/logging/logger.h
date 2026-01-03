#pragma once
// This class manages and directs log messages to different
// `LogPipe`s.

#include "Engine/logging/log_pipe.h"
#include <map>
#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

using std::map;
using std::string;
using std::vector;
using std::thread;
using std::queue;

struct LogMessage {
	string message;
	string pipe_name;
	Log::Domain domain;
	Log::Severity severity;
	std::tm time;
};

class Logger {
public:
	Logger(vector<LogPipe*> pipes);
	~Logger();
	

	void log(string message, string pipe_name, Log::Domain domain, Log::Severity severity);
	void flush();
private:
	static void thread_main(Logger* self);

	void serial_log(const LogMessage & log);

	void throw_error(string msg);

	queue<LogMessage> log_queue;
	map<string, LogPipe*> pipe_map;
	thread logging_thread;
	bool thread_running = false;
	std::condition_variable cv;
	std::condition_variable flush_cv;
	std::mutex mtx;
	bool errored = false;
	string error_msg;
};