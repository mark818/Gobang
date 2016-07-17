#include <WinSock2.h>
#include "network.h"
#include "ui.h"

UI* Network::ui = 0;

Network::Network(char* ipad, int port, UI *p) : port(port), quit(false)  {
	ui = p;
	strcpy(ip, ipad);
}

bool Network::ClientInit() {
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

bool Network::ClientConnect() {

	if (connect(clt, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in))) {
		char a[50];
		sprintf(a, "%u", WSAGetLastError());
		MessageBoxA(0, a, "Connect failed", MB_OK);
		return 1;
	}
	send(clt, "connect", 100, 0);
	//CreateThread(0, 0, PullFromServer, this, 0, 0);
	return 0;
}

void Network::SendMsg(const char* msg) {
	send(clt, msg, 100, 0);
}

bool Network::Close() {
	send(clt, "t", 1, 0);
	quit = true;
	closesocket(clt);
	WSACleanup();
	return 0;
}

