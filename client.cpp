#include "client.h"

#include <string>

DWORD WINAPI GetNewMessage(LPVOID client_socket);
DWORD WINAPI ServerMessage(LPVOID client_socket);

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
		std::cout << "Connect error. The server is currently offline!"<< WSAGetLastError() << std::endl;
		return ;
	}

	std::cout << "Successful connection with "<< SERVERADDR<<" \n\Enter 'quit' for quit" << std::endl;
	start();
}


Client::~Client()
{
}


void Client::start() {
	//system("clear");

	std::string answer;
	std::cout << "Welcome to the chat client!" << std::endl;
	std::cout << "Type \\create to create a new account or enter in your credentials to log in.\n\n";

	while (true)
	{
		std::cout << "Username: ";
		std::getline(std::cin, username);

		if (username == "\\create") {
			createAccount();
		}

		std::cout << "Password: ";
		//setPasswordInput(true);
		std::getline(std::cin, password);
		//setPasswordInput(false);

		std::string loginStr = client_login(username, password);
		send(my_sock, loginStr.c_str(), loginStr.length(), 0);
		char response;
		recv(my_sock, &response, sizeof(response), 0);

		if (response == ERROR_ACCOUNT_DOES_NOT_EXIST || response == ERROR_INCORRECT_PASSWORD) {
			std::cout << "The username or password you entered is incorrect! Please try again.\n\n";
		}
		else {
			system("clear");
			break;
		}
	}

	//mainMenu();
}


void Client::createAccount() {
	std::cout << "Enter your desired username: ";
	std::getline(std::cin, username);
	std::cout << "Enter a password: ";

	//setPasswordInput(true);
	std::getline(std::cin, password);
	//setPasswordInput(false);

	std::cout << "\n";

	std::cout << "Enter your first name: ";
	std::getline(std::cin, firstName);
	std::cout << "Enter your last name: ";
	std::getline(std::cin, lastName);

	std::string create = client_createAccount(username, password, firstName, lastName);
	send(my_sock, create.c_str(), create.length(), 0);

	char response;
	recv(my_sock, &response, sizeof(response), 0);

	if (response == CREATE_ACCOUNT_SUCCESS) {
		std::string loginStr = client_login(username, password);
		send(my_sock, loginStr.c_str(), loginStr.length(), 0);
		char response;
		recv(my_sock, &response, sizeof(response), 0);
		system("clear");
	}
}



void Client::handle()
{
	DWORD thID;
	CreateThread(NULL, NULL, ServerMessage, &my_sock, NULL, &thID);

	int nsize;
	while (true)
	{
		//buff[nsize] = 0;

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
			send(my_sock, &buff[0], sizeof(buff), 0);
		}
	}


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
		//Sleep(2000);
	}
	std::cout << "Recv error " << WSAGetLastError() << std::endl;
	return 0;
}

DWORD WINAPI ServerMessage(LPVOID client_socket)
{

	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];
	int nsize;
	char buff[1024];

	SOCKET client_socketT;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	SOCKET mysocket = socket(AF_INET, SOCK_STREAM, 0);

	recv(my_sock, &buff[0], sizeof(buff) - 1, 0);
	std::cout << "S=>C:" << buff << std::endl;

	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{

		buff[nsize] = 0;

		std::cout << "S=>C:" << buff << std::endl;
		client_socketT = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size);
			HOSTENT *hst;
			hst = gethostbyaddr((char *)
				&client_addr.sin_addr.s_addr, 4, AF_INET);

			std::cout << "+" << "new connect!\n" << inet_ntoa(client_addr.sin_addr) << std::endl;

			DWORD thID;
			CreateThread(NULL, NULL, GetNewMessage, &client_socketT, NULL, &thID);


			
		//Sleep(2000);
	}
	std::cout << "Recv error " << WSAGetLastError() << std::endl;
	return 0;
}
