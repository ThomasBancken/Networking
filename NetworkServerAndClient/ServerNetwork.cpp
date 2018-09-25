#include "stdafx.h" 
#include "ServerNetwork.h"
#include "NetworkData.h"

ServerNetwork::ServerNetwork(void)
{
	WSADATA wsaData;

	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	// address info for the server to listen to
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	m_IResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (m_IResult != 0) {
		printf("WSAStartup failed with error: %d\n", m_IResult);
		exit(1);
	}

	// set address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP; 
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	m_IResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (m_IResult != 0) {
		printf("getaddrinfo failed with error: %d\n", m_IResult);
		WSACleanup();
		exit(1);
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}

	// Set the mode of the socket to be nonblocking to let the program remain in control
	u_long iMode = 1;
	m_IResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);

	if (m_IResult == SOCKET_ERROR) {
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// Setup the listening socket
	m_IResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (m_IResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// no longer need address information
	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	m_IResult = listen(ListenSocket, SOMAXCONN);

	if (m_IResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
}

// accept new connections
bool ServerNetwork::AcceptNewClient(unsigned int & id)
{
	// if client waiting, accept the connection and save the socket
	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket != INVALID_SOCKET)
	{
		//disable nagle on the client's socket to improve responde time
		char value = 1;
		setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		m_Sessions.insert(pair<unsigned int, SOCKET>(id, ClientSocket));

		return true;
	}

	return false;
}

int ServerNetwork::ReceiveData(unsigned int client_id, char * recvbuf)
{
	if (m_Sessions.find(client_id) != m_Sessions.end())
	{
		SOCKET currentSocket = m_Sessions[client_id];
		m_IResult = NetworkServices::ReceiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);
		if (m_IResult == 0)
		{
			printf("Connection closed\n");
			closesocket(currentSocket);
		}
		return m_IResult;
	}
	return 0;
}

void ServerNetwork::SendToAll(char * packets, int totalSize)
{
	SOCKET currentSocket;
	std::map<unsigned int, SOCKET>::iterator iter;
	int iSendResult;

	for (iter = m_Sessions.begin(); iter != m_Sessions.end(); iter++)
	{
		currentSocket = iter->second;
		iSendResult = NetworkServices::sendMessage(currentSocket, packets, totalSize);

		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(currentSocket);
		}
	}
}

