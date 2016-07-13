#pragma once
#include "game.h"
//#include "network.h"
#include "stdafx.h"
#include <string>

class UI {
	friend class Chat_Client;
public:
	UI(HINSTANCE hinst, int nCmdShow) noexcept;
	~UI();
	UI(const UI&) = delete;
	UI(const UI&&) = delete;
	UI& operator=(const UI &) = delete;
	void run() noexcept;
	
private:
	const static char BLACK = 1, WHITE = 2, SINGLE = 1, MULT = 2, REVIEW = 3;
	char mode, color, pause, text[100], terminate;
	static UI *self;
	CRITICAL_SECTION crit;
	HANDLE event, msgposter;
	static HDC memdc;
	static void postmsg() noexcept;
	HINSTANCE hinst;
	HWND hwnd, hlist, hedit;
	TCHAR szTitle[16];					// 标题栏文本
	TCHAR szWindowClass[16];			// 主窗口类名
	ULONG_PTR m_gdiplusToken;
	static void put(char hor, char ver);
	static LRESULT _stdcall WndProc(HWND, UINT, WPARAM, LPARAM) ;
	static LRESULT _stdcall About(HWND, UINT, WPARAM, LPARAM) ;
	static LRESULT _stdcall EDIT(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR) ;
	Game game;
	//Chat_Client net;
};