#include "stdafx.h"
#include "ui.h"
#include "resource.h"
#include <string>

UI* UI::self = 0;
HDC UI::memdc = 0;

UI::UI(HINSTANCE hinst, int nCmdShow) noexcept : hinst(hinst), mode(SINGLE),  /* net("192.168.1.1",2225, this), */
szWindowClass(L"GO"), szTitle(L"五子棋"), color(BLACK), pause(0) {
	self = this;
	InitializeCriticalSection(&self->crit);
	event = CreateEvent(0, 0, 0, 0);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinst;
	wcex.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_GO));
	wcex.hCursor = LoadCursor(NULL, IDC_HAND);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_GO);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(hinst, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);

	hwnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, 0, 1200, 800, NULL, NULL, hinst, NULL);

	if (!hwnd) {
		DWORD error = GetLastError();
		MessageBoxA(0, std::to_string(error).c_str(), "初始化", MB_ICONERROR | MB_OK);
		exit(0);
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

void UI::run() noexcept {
	MSG msg;
	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hinst, MAKEINTRESOURCE(IDC_GO));
	msgposter = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)postmsg, 0, 0, 0);
	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

LRESULT  UI::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static PAINTSTRUCT ps;
	static HBITMAP hmap;
	static RECT rect;
	switch (msg) {
		case WM_CREATE: {
			
			self->hlist = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_AUTOVSCROLL,
																900, 0, 285, 680, hwnd, 0, self->hinst, 0);
			self->hedit = CreateWindowA("EDIT", "在此输入聊天内容", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL,
																900, 680, 285, 80, hwnd, 0, self->hinst, 0);
			SetWindowSubclass(self->hedit, (SUBCLASSPROC)EDIT, 2, 0);
			PostMessage(hwnd, WM_COMMAND, static_cast<WPARAM>(IDM_RESTART), 0);
			LOGFONTA logfont = { 0 };
			strcpy_s(logfont.lfFaceName, 15, "Kaiti");
			HFONT hfont = CreateFontIndirectA(&logfont);
			PostMessage(self->hlist, WM_SETFONT, (WPARAM)hfont, 0);
			PostMessage(self->hedit, WM_SETFONT, (WPARAM)hfont, 0);
		}
		break;
		case WM_COMMAND: {
			int event = HIWORD(wparam);
			int id = LOWORD(wparam);
			switch (id) {
				case ID_QUIT: {
					goto quit;
				}
				//case ID_ONLINE: {
				//	SendMessage(hwnd, WM_COMMAND, static_cast<WPARAM>(IDM_RESTART), 0);
				//	self->mode = MULT;
				//	self->net.ClientInit();
				//	self->net.ClientConnect();
				//}
				case IDM_ABOUT:
						DialogBox(self->hinst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
						break;
				case IDM_RESTART: {
					if (!self->pause) {
						HDC hdc = GetDC(hwnd);
						GetClientRect(hwnd, &rect);

						memdc = CreateCompatibleDC(hdc);
						HBITMAP hmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
						ReleaseDC(hwnd, hdc);
						SelectObject(memdc, hmap);
					}
					Gdiplus::Image bg(L"bg.jpg");
					Gdiplus::Graphics graphics(memdc);
					graphics.DrawImage(&bg, 0, 0, rect.right, rect.bottom);
					
					Gdiplus::SolidBrush br(Gdiplus::Color(Gdiplus::Color::MakeARGB(230, 213, 176, 146)));
					Gdiplus::RectF r(50, 50, 660, 660);
					graphics.FillRectangle(&br, r);

					SetBkMode(memdc, TRANSPARENT);
					char a, b[3];
					for (int i = 100, k = 0; i < 700; i += 42, k++) {
						MoveToEx(memdc, 100, i, 0);
						LineTo(memdc, 688, i);
						MoveToEx(memdc, i, 100, 0);
						LineTo(memdc, i, 688);
						a = 'A' + k;
						_itoa_s(1 + k, b, 3, 10);
						TextOutA(memdc, i, 60, &a, 1);
						TextOutA(memdc, 60, i - 5, b, k < 9 ? 1 : 2);
					}
					static HBRUSH black = CreateSolidBrush(0);
					SelectObject(memdc, black);
					const int p = 100 + 7 * 42;
					Ellipse(memdc, p - 5, p - 5, p + 5, p + 5);
					const int p2 = 100 + 3 * 42;
					Ellipse(memdc, p2 - 5, p2 - 5, p2 + 5, p2 + 5);
					const int p3 = 100 + 11 * 42;
					Ellipse(memdc, p3 - 5, p3 - 5, p3 + 5, p3 + 5);
					Ellipse(memdc, 100 + 11 * 42 - 5, 100 + 3 * 42 - 5, 100 + 11 * 42 + 5, 100 + 3 * 42 + 5);
					Ellipse(memdc, 100 + 3 * 42 - 5, 100 + 11 * 42 - 5, 100 + 3 * 42 + 5, 100 + 11 * 42 + 5);
					self->pause = 0;
					self->color = BLACK;
					self->mode = SINGLE;
					self->game.reset();
					InvalidateRect(hwnd, &rect, true);
					break;
				}
				case ID_REVIEW: {
					WndProc(self->hwnd, WM_COMMAND, static_cast<WPARAM>(IDM_RESTART), 0);
					if (self->game.open_saved()) 
						self->mode = REVIEW;
				}
			}
		}
		break;
		case WM_LBUTTONDOWN: {
			if (self->pause)
				return 0;
			if (self->mode == REVIEW) {
				std::pair<char, char> p;
				if (self->game.read_next(p)) {
					put(p.first, p.second);
					self->color = 3 - self->color;
				} else {
					self->pause = 1;
					if (self->color = WHITE)
						MessageBox(self->hwnd, L"黑方赢", L"复盘结束", MB_OK);
					else
						MessageBox(self->hwnd, L"白方赢", L"复盘结束", MB_OK);
				}
				return 0;
			}
			int x = LOWORD(lparam) - 100;
			int y = HIWORD(lparam) - 100;
			if (x < 0 || y < 0)
				return 0;
			int xr = x % 42, yr = y % 42;
			if (xr > 15 && xr < 27 || yr > 15  && yr < 27)
				return 0;
			char hor = xr < 16 ? x / 42 : x / 42 + 1, ver = yr < 16 ?  y / 42 : y / 42 + 1;
			if (hor > 14 || ver > 14)
				break;
			int re = self->game.put(hor, ver, self->color);
			if (re == -1)
				return 0;
			put(hor, ver);
			switch (re) {
			case 1:
				self->pause = 1;
				if (MessageBox(self->hwnd, L"是否存盘？", L"黑方赢", MB_YESNO) == IDYES)
					self->game.save();
				break;
			case 2:
				self->pause = 1;
				if (MessageBox(self->hwnd, L"是否存盘？", L"白方赢", MB_YESNO) == IDYES)
					self->game.save();
			default:
				self->color = 3 - self->color;
			}
			return 0;
		}
		break;
		case WM_PAINT: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			HDC hdc = BeginPaint(hwnd, &ps);
			BitBlt(hdc, 0, 0, rect.right, rect.bottom, memdc, 0, 0, SRCCOPY);
			EndPaint(hwnd, &ps);
		}
		break;
		case WM_CLOSE:
quit: case WM_DESTROY:
			DeleteObject(hmap);
			DeleteDC(memdc);
			self->terminate = 1;
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

void UI::put(char hor, char ver) {
	
	int x = hor * 42 + 100, y = ver * 42 + 100;
	static HBRUSH black = CreateSolidBrush(0);
	static HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
	switch (self->color) {
	case BLACK:
		SelectObject(memdc, black);
		break;
	case WHITE:
		SelectObject(memdc, white);
	}
	Ellipse(memdc, x - 14, y - 14, x + 14, y + 14);
	RECT rect = { x - 14, y - 14, x + 14, y + 14 };
	InvalidateRect(self->hwnd, &rect, false);

	
}

void UI::postmsg() noexcept {
	int count = 0;
	DWORD l, r;
	while (!self->terminate) {
		WaitForSingleObject(self->event, INFINITE);
		SendMessage(self->hlist, EM_GETSEL, (WPARAM)&l, (LPARAM)&r);
		SendMessage(self->hlist, EM_SETSEL, count, count);
		SendMessageA(self->hlist, EM_REPLACESEL, 0, (LPARAM)self->text);
		count += strlen(self->text);
		LeaveCriticalSection(&self->crit);
	}
	SetEvent(self->event);
}

LRESULT _stdcall UI::EDIT(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	if (msg == WM_KEYDOWN && LOWORD(wparam) == VK_RETURN) {
		EnterCriticalSection(&self->crit);
		GetWindowTextA(hwnd, self->text, 100);
		SetEvent(self->event);		
		SetWindowTextA(hwnd, 0);
		return 0;
	}
	return DefSubclassProc(hwnd, msg, wparam, lparam);
}

UI::~UI() {
	WaitForSingleObject(&event, INFINITE);
	CloseHandle(msgposter);
	CloseHandle(event);
	DeleteCriticalSection(&crit);
	Gdiplus::GdiplusShutdown(m_gdiplusToken); 
}

LRESULT __stdcall UI::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}