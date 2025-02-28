#include<iostream>
#include<winsock2.h>  //library for winsocket
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;


/*1. initialize winsock library
2. create the socket
3. get ip and port
4. bind the ip / port with the socket
5. listen on the socket
6. accept
7. recv and send
8. close the socket
9. clean up the winsock*/


#pragma comment(lib,"ws2_32.lib")


bool initilization() {
	WSADATA data; //structure contains information about the Windows Sockets implementation.
	return(WSAStartup(MAKEWORD(2, 2), &data) == 0); // must be the first Windows Sockets function called by an application or DLL.
}

void InterectWithClient(SOCKET clientSocket, vector<SOCKET>& clients)
{
	cout << "Client connected" << endl;
	char buffer[4096];
	//Receive
	while (1)
	{
		int bytesrevd = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesrevd <= 0) {
			cout << "Client Disconnect" << endl;
			break;
		}
		string message(buffer, bytesrevd);
		cout << "Message from client : " << message << endl;

		for (auto client : clients)
		{
			if(client!= clientSocket)
			send(client, message.c_str(), message.length(), 0);
		}
	}
	
	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}
	
	closesocket(clientSocket);
}



int main()
{
	if (initilization())//check if it been initilized
	{
		cout << "lib initilized"<<endl;
	}							
	else						
	{							
		cout << "Not initilized"<<endl;
		return 1;
	}
	
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)//check if client side socket is working
	{
		cout << "FAILED SOCKET CREATION!!!!" << endl;
		return 1;
	}

	//creating address structure
	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET; //
	serveraddr.sin_port = htons(port);

	//convert the ip  address to binary format
	//putting ip(0.0.0.0) inside the sin_family in binary
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
	{
		cout << "Setting address structure failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//Binding 
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr) )== SOCKET_ERROR)
	{
		cout << "Bind failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}

	//Listen
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)//AMOUNT OF REQUEST BEFORE GIVING LIMIT
	{
		cout << "Listening failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	cout << "Server has started we are listening to the port : "<<port <<endl;
	vector<SOCKET> clients;
	while (1)
	{
		//Accept
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "INVALID CLIENT SOCKET" << endl;
		}

		clients.push_back(clientSocket);
		thread t1(InterectWithClient, clientSocket , std::ref(clients));
		t1.detach();
	}
	
	
	closesocket(listenSocket);

	WSACleanup();
	return 0;
}