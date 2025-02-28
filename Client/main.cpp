#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include<thread>
#pragma comment(lib, "Ws2_32.lib") 
using namespace std;

/*
	initalize winsock

	create socket

	connect to the server

	send/rev

	close the socket

*/

bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendsMessage(SOCKET s)
{
	cout << "Enter your Chat Name : " << endl;
	string name;
	getline(cin, name);
	string message;
	while (1) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR) {
			cout << "Error sending messgae" << endl;
			break;
		}

		if (message == "quit") {
			cout << "Stopping the application" << endl;
			break;
		}
	}
	closesocket(s);
	WSACleanup();
}


void ReceiveMessage(SOCKET s)
{
	char buffer[4096];
	int revlength;
	string msg;
	while (1) {
		revlength=recv(s, buffer, sizeof(buffer), 0);
		if (revlength <= 0) {
			cout << "Disconnected from program" << endl;
			break;
		}
		else {
			msg = string(buffer, revlength);
			cout <<"                                                           "<< msg << endl;
		}
	}
	closesocket(s);
	WSACleanup();

}

int main() {

	if (!Initialize()) {
		cout << "Not initialized" << endl;
		return 1;
	}


	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "Invalid socket created" << endl;
		return 1;
	}



	int port = 12345;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));


	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "NOt able to connect to server" << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}
	cout << " successfully connected to server " << endl;

	thread Senderthread(SendsMessage, s);
	thread Receivethread(ReceiveMessage, s);

	Senderthread.join();
	Receivethread.join();

	


	return 0;
}