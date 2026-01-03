#include "Engine/logging/logger.h"
#include <stdexcept>



Logger::Logger(vector<LogPipe*> pipes) {
	for (auto pipe : pipes) {
		this->pipe_map.insert(std::make_pair(pipe->name, pipe));
	}

	this->thread_running = true;
	this->logging_thread = thread(thread_main, this);
}

Logger::~Logger() {
	{
		std::lock_guard<std::mutex> lock(mtx);
		this->thread_running = false;
	};
	cv.notify_one();

	this->logging_thread.join();

	if (errored)
		throw std::runtime_error(error_msg);
}

void Logger::thread_main(Logger* self) {
	while (true) {
		std::unique_lock<std::mutex> lock(self->mtx);

		self->cv.wait(lock, [self] { return !self->log_queue.empty() || !self->thread_running; });

		if (!self->thread_running && self->log_queue.empty())
			break;

		if (self->log_queue.empty())
			continue;

		LogMessage log = self->log_queue.front();
		self->log_queue.pop();
		
		lock.unlock();

		self->serial_log(log);
		
		lock.lock();
		if (self->log_queue.empty()) {
			self->flush_cv.notify_all();
		}
	}
}

void Logger::serial_log(const LogMessage& log) {
	if (pipe_map.find(log.pipe_name) != pipe_map.end()) {
		pipe_map.at(log.pipe_name)->log(log.message, log.domain, log.severity, log.time);
	}
	else {
		this->throw_error(log.pipe_name + " logging pipe does not exist.");
	}
}

void Logger::throw_error(string msg) {
	{
		std::lock_guard<std::mutex> lock(mtx);
		this->thread_running = false;
		error_msg = msg;
		errored = true;
	};
}

void Logger::log(string message, string pipe_name, Log::Domain domain, Log::Severity severity) {
	system_clock::time_point now = std::chrono::system_clock::now();

	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};

	tm = *std::localtime(&time);

	{
		std::lock_guard<std::mutex> lock(mtx);

		this->log_queue.push(
			LogMessage{
				message,
				pipe_name,
				domain,
				severity,
				tm
			}
		);
	}
	cv.notify_one();
}

void Logger::flush() {
	std::unique_lock<std::mutex> lock(mtx);
	cv.notify_one();
	flush_cv.wait(lock, [this] { return log_queue.empty() || !thread_running; });
}