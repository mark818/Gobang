#include "logger.h"
#include "stdafx.h"
using namespace std;

Logger::Logger() {
	log.reserve(255);
}

bool Logger::parse_log() noexcept {
	OPENFILENAMEA file;
	ZeroMemory(&file, sizeof(OPENFILENAME));
	file.lStructSize = sizeof(OPENFILENAME);//结构体大小  
											//设置过滤  
	file.lpstrFilter = "五子棋存盘文件 (.godat)\0*.godat\0所有文件\0*.*\0";
	//默认过滤器索引设为1  
	file.nFilterIndex = 1;
	//文件名的字段必须先把第一个字符设为 \0 
	char strFilename[200];
	file.lpstrFile = static_cast<LPSTR>(strFilename);
	file.lpstrFile[0] = '\0';
	file.nMaxFile = sizeof(strFilename);
	//设置标志位，检查目录或文件是否存在  
	file.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	GetOpenFileNameA(&file);
	HANDLE f = CreateFileA(static_cast<LPCSTR>(strFilename), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (f == INVALID_HANDLE_VALUE) {
		MessageBoxA(0, "无法打开文件", "日志错误", MB_OK);
		return false;
	}

	char buf[256];
	DWORD len;
	if (!ReadFile(f, buf, 256, &len, 0)) {
		char str[20];
		sprintf(str, "%d", GetLastError());
		MessageBoxA(0, str, "后台日志错误", MB_OK);
		CloseHandle(f);
		return false;
	}
	CloseHandle(f);
	log.clear();
	log.reserve(len);
	for (int i = 0; i < len; i++) {
		log.push_back(buf[i]);
	}
	count = 0;
	return true;
}

bool Logger::read_next(pair<char, char> &pair) noexcept {
	if (count == log.size())
		return false;
	unsigned char val = log[count++];
	pair = { val % 15, val / 15};
	return true;
}

void Logger::record(unsigned char step) noexcept {
	log.push_back(step);
}

bool Logger::write_to_file() noexcept {
	OPENFILENAMEA file;
	ZeroMemory(&file, sizeof(OPENFILENAME));
	file.lStructSize = sizeof(OPENFILENAME);//结构体大小  
											//设置过滤  
	file.lpstrFilter = "五子棋存盘文件 (.godat)\0*.godat\0所有文件\0*.*\0";
	//默认过滤器索引设为1  
	file.nFilterIndex = 1;
	//文件名的字段必须先把第一个字符设为 \0 
	char strFilename[200];
	file.lpstrFile = static_cast<LPSTR>(strFilename);
	file.lpstrFile[0] = '\0';
	file.nMaxFile = sizeof(strFilename);
	//设置标志位，检查目录或文件是否存在  
	file.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_SHOWHELP;
	GetSaveFileNameA(&file);
	HANDLE f = CreateFileA(file.lpstrFile, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (f == INVALID_HANDLE_VALUE)
		return false;
	DWORD written;
	if (!WriteFile(f, static_cast<LPVOID>(&log[0]), log.size(), &written, 0)) {
		CloseHandle(f);
		return false;
	}
	CloseHandle(f);
	return true;
}

void Logger::reset() noexcept {
	log.clear();
}
