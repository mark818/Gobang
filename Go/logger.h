#pragma once
#include <vector>
class Logger
{
public:
	Logger();
	Logger(const Logger &) = delete;
	Logger(const Logger &&) = delete;
	Logger& operator= (const Logger &) = delete;
	~Logger() = default;

	bool parse_log() noexcept;
	bool read_next(std::pair<char, char>&) noexcept;
	void record(unsigned char step) noexcept;
	bool write_to_file() noexcept;
	void reset() noexcept;
	
private:
	std::vector<unsigned char> log;
	unsigned char count = 0;
};

