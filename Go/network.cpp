#include "network.h"
#include "ui.h"

UI* Chat_Client::ui = 0;

Chat_Client::Chat_Client(char* ipad, int port, UI *p) : port(port), quit(false)  {
	ui = p;
	strcpy(ip, ipad);
}

bool Chat_Client::ClientInit() {
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(ip);

	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		MessageBoxA(0, "Socket startup failed", "Connection Failed", MB_OK);
		return 1;
	}

	clt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clt == INVALID_SOCKET) {
		MessageBoxA(0, "Invalid socket", "Connection Failed", MB_OK);
		return 1;
	}
	return 0;
}

bool Chat_Client::ClientConnect() {

	if (connect(clt, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in))) {
		char a[50];
		sprintf(a, "%u", WSAGetLastError());
		MessageBoxA(0, a, "Connect failed", MB_OK);
		return 1;
	}
	send(clt, "connect", 100, 0);
	if (_beginthread(PullFromServer, 0, (void*)this) == -1L) {
		MessageBoxA(0, "Error creating worker thread", "Connect failed", MB_OK);
		return 1;
	}
	return 0;
}

void Chat_Client::SendMsg(const char* msg) {
	send(clt, msg, 100, 0);
}

bool Chat_Client::Close() {
	send(clt, "t", 1, 0);
	quit = true;
	closesocket(clt);
	WSACleanup();
	return 0;
}

void __stdcall Chat_Client::PullFromServer(void* data) {
	Chat_Client* self = static_cast<Chat_Client*>(data);
	char buffer[100];
	while (!self->quit && recv(self->clt, buffer, 100, 0) > 0) {
		if (strcmp(buffer, "chat")) {
			recv(self->clt, buffer, 100, 0);
			EnterCriticalSection(&(ui->crit));
			strcpy_s(ui->text, 100, buffer);
			SetEvent(ui->event);
		}
		else if (strcmp(buffer, "put")) {
			recv(self->clt, buffer, 100, 0);
			ui->put(buffer[0], buffer[1]);
		}
		else if (strcmp(buffer, "quit")) {
			self->quit = true;
		}
	}
	self->quit = true;
}

