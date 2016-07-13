#include "logger.h"
#include "stdafx.h"
using namespace std;

Logger::Logger() {
	log.reserve(255);
}

bool Logger::parse_log() noexcept {
	OPENFILENAMEA file;
	ZeroMemory(&file, sizeof(OPENFILENAME));
	file.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
											//���ù���  
	file.lpstrFilter = "����������ļ� (.godat)\0*.godat\0�����ļ�\0*.*\0";
	//Ĭ�Ϲ�����������Ϊ1  
	file.nFilterIndex = 1;
	//�ļ������ֶα����Ȱѵ�һ���ַ���Ϊ \0 
	char strFilename[200];
	file.lpstrFile = static_cast<LPSTR>(strFilename);
	file.lpstrFile[0] = '\0';
	file.nMaxFile = sizeof(strFilename);
	//���ñ�־λ�����Ŀ¼���ļ��Ƿ����  
	file.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	GetOpenFileNameA(&file);
	HANDLE f = CreateFileA(static_cast<LPCSTR>(strFilename), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (f == INVALID_HANDLE_VALUE) {
		MessageBoxA(0, "�޷����ļ�", "��־����", MB_OK);
		return false;
	}

	char buf[256];
	DWORD len;
	if (!ReadFile(f, buf, 256, &len, 0)) {
		char str[20];
		sprintf(str, "%d", GetLastError());
		MessageBoxA(0, str, "��̨��־����", MB_OK);
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
	file.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
											//���ù���  
	file.lpstrFilter = "����������ļ� (.godat)\0*.godat\0�����ļ�\0*.*\0";
	//Ĭ�Ϲ�����������Ϊ1  
	file.nFilterIndex = 1;
	//�ļ������ֶα����Ȱѵ�һ���ַ���Ϊ \0 
	char strFilename[200];
	file.lpstrFile = static_cast<LPSTR>(strFilename);
	file.lpstrFile[0] = '\0';
	file.nMaxFile = sizeof(strFilename);
	//���ñ�־λ�����Ŀ¼���ļ��Ƿ����  
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
