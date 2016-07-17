#pragma once
#include "stdafx.h"
#include <process.h>

#include <iostream>

#pragma comment(lib, "ws2_32.lib")

class UI;

class Network {
public:
	Network(char*, int, UI*);
	bool ClientInit();
	bool ClientConnect();
	void SendMsg(const char*);
	bool Close();
private:
	static UI *ui;
	char ip[16];
	u_short port;
	WSADATA wsaData;
	sockaddr_in addr;
	SOCKET clt;
	HANDLE ht;
	bool quit;
};