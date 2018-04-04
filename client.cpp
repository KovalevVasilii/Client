#include "client.h"


DWORD WINAPI GetNewMessage(LPVOID client_socket);

Client::Client()
{
	setlocale(LC_ALL, "RUS");
	std::cout<<"TCP DEMO CLIENT"<<std::endl;
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		std::cout << "WSAStart error "<< WSAGetLastError() << std::endl;
		return ;
	}
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		std::cout << "Socket() error"<<WSAGetLastError() << std::endl;
		return;
	}
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(MY_PORT);
	HOSTENT *hst;

	if (inet_addr(SERVERADDR) != INADDR_NONE)
	{
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	}
	else
	{
		if (hst = gethostbyname(SERVERADDR))
		{
			((unsigned long *)&dest_addr.sin_addr)[0] =((unsigned long **)hst->h_addr_list)[0][0];
		}
		else
		{
			std::cout << "Invalid address"<<SERVERADDR << std::endl;
			closesocket(my_sock);
			WSACleanup();
			return ;
		}

	}
	if (connect(my_sock, (sockaddr *)&dest_addr,sizeof(dest_addr)))
	{
		std::cout << "Connect error "<< WSAGetLastError() << std::endl;
		return ;
	}

	std::cout << "Connection with "<< SERVERADDR<<" is success\n\Enter 'quit' for quit" << std::endl;
	handle();
}


Client::~Client()
{
}

void Client::handle()
{
	DWORD thID;
	//CreateThread(NULL, NULL, GetNewMessage, &my_sock, NULL, &thID);

	int nsize;
	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{
		buff[nsize] = 0;

		std::cout << "S=>C:"<< buff << std::endl;

		std::cout << "S<=C:"; 
		fgets(&buff[0], sizeof(buff) - 1, stdin);

		if (!strcmp(&buff[0], "quit\n"))
		{
			std::cout << "Exit..." << std::endl;
			closesocket(my_sock);
			WSACleanup();
			return ;
		}
		if (sizeof(buff) > 0)
		{
			send(my_sock, &buff[0], nsize, 0);
		}
	}

	std::cout << "Recv error "<<WSAGetLastError() << std::endl;
	closesocket(my_sock);
	WSACleanup();
	return ;
}

DWORD WINAPI GetNewMessage(LPVOID client_socket)
{

	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];
	int nsize;
	char buff[1024];

	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{

		buff[nsize] = 0;

		std::cout << "S=>C:"<< buff << std::endl;
		Sleep(2000);
	}
	return 0;
}

