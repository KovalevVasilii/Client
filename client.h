#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#include <iostream>
#include <WinSock2.h>
#include <winsock.h>
#define MY_PORT    666
#define SERVERADDR "127.0.0.1"

class Client
{
public:
	Client();
	~Client();
	void handle();
private:
	SOCKET my_sock;
	SOCKADDR_IN dest_addr;
	char buff[1024];
};

#endif