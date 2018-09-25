#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "NetworkServices.h"
#include <ws2tcpip.h>
#include <map>

using namespace std;
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "6881"

class ServerNetwork
{

public:

	ServerNetwork(void);
	~ServerNetwork(void);

	// Socket to listen for new connections
	SOCKET ListenSocket;

	// Socket to give to the clients
	SOCKET ClientSocket;

	// for error checking return values
	int m_IResult;

	// table to keep track of each client's socket
	std::map<unsigned int, SOCKET> m_Sessions;

	// accept new connections
	bool AcceptNewClient(unsigned int & id);

	// receive incoming data
	int ReceiveData(unsigned int client_id, char * recvbuf);

	// send data to all clients
	void SendToAll(char * packets, int totalSize);
};






