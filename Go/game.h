#include "stdafx.h"
#include "logger.h"

class Game {
public:
	Game();
	void reset();
	int put(char hor, char ver, char color);
	void save();
	bool open_saved();
	bool read_next(std::pair<char, char> &);
	~Game();
private:
	int isOver(char hor, char ver, char color);
	char *board;
	Logger log;
};