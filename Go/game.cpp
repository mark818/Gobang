#include "game.h"

Game::Game() : board(new char[15 * 15]) {
}

void Game::reset() {
	memset(board, 0, 15 * 15);
	log.reset();
}

int Game::put(char hor, char ver, char color) {
	unsigned char index = ver * 15 + hor;
	if (board[index])
		return -1;
	board[index] = color;
	log.record(index);
	return isOver(hor, ver, color);
}

Game::~Game() {
	delete[] board;
}

inline bool proc(char dat, char color, char &count) {
	if (dat == color) {
		if (++count == 5) 
			return true;
		return false;
	}
	else {
		count = 0;
		return false;
	}
}

int Game::isOver(char hor, char ver, char color) {
	char hor_left = hor - 4 < 0 ? 0 : hor - 4;
	char hor_right = hor + 4 > 14 ? 14 : hor + 4;
	char ver_top = ver - 4 < 0 ? 0 : ver - 4;
	char ver_bot = ver + 4 > 14 ? 14 : ver + 4;
	
	char count = 0; 
	for (char cur = hor_left; cur <= hor_right; cur++) {
		if (proc(board[ver * 15 + cur], color, count)) 
			return color;
	}

	count = 0;
	for (char cur = ver_top; cur <= ver_bot; cur ++) {
		if (proc(board[cur * 15 + hor], color, count))
			return color;
	}

	count = 0; 
	char x_start = hor - min(hor - hor_left, ver - ver_top),
		    y_start = ver - min(hor - hor_left, ver - ver_top),
		    x_stop = hor + min(hor_right - hor, ver_bot - ver),
		    y_stop = ver + min(hor_right - hor, ver_bot - ver);
	for (; x_start <= x_stop && y_start <= y_stop; x_start++, y_start++) {
		if (proc(board[y_start * 15 + x_start], color, count))
			return color;
	}

	count = 0; 
	x_start = hor + min(hor_right - hor, ver - ver_top),
	y_start = ver - min(hor_right - hor, ver - ver_top),
	x_stop = hor - min(hor - hor_left, ver_bot - ver),
	y_stop = ver + min(hor - hor_left, ver_bot - ver);
	for (; x_start >= x_stop && y_start <= y_stop; x_start--, y_start++) {
		if (proc(board[y_start *15 + x_start], color, count))
			return color;
	}
	return 0;
}

void Game::save() {
	log.write_to_file();
}

bool Game::open_saved() {
	return log.parse_log();
}

bool Game::read_next(std::pair<char, char>& p) {
	return log.read_next(p);
}
