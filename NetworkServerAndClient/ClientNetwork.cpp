#include "stdafx.h"
#include "ClientNetwork.h"
#include "NetworkData.h"

ClientNetwork::ClientNetwork(void) {

	WSADATA wsaData;

	ConnectSocket = INVALID_SOCKET;

	// holds address info for socket to connect to
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Initialize Winsock
	m_IResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (m_IResult != 0) {
		printf("WSAStartup failed with error: %d\n", m_IResult);
		exit(1);
	}

	// set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  
									  
	//resolve server address and port
	m_IResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);

	if (m_IResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", m_IResult);
		WSACleanup();
		exit(1);
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			exit(1);
		}

		// Connect to server.
		m_IResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (m_IResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			printf("The server is down... did not connect");
		}
	}

	// no longer need address info for server
	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		exit(1);
	}

	// Set the mode of the socket to be nonblocking to let the program remain in control
	u_long iMode = 1;

	m_IResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (m_IResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		exit(1);
	}

	//disable nagle to improve responde time
	char value = 1;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

}

int ClientNetwork::ReceivePackets(char * recvbuf)
{
	m_IResult = NetworkServices::ReceiveMessage(ConnectSocket, recvbuf, MAX_PACKET_SIZE);

	if (m_IResult == 0)
	{
		printf("Connection closed\n");
		closesocket(ConnectSocket);
		WSACleanup();
		exit(1);
	}

	return m_IResult;
}

bool ClientNetwork::CloseConnection()
{
	m_IResult = shutdown(ConnectSocket, SD_SEND);
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Close Failed\n");
		closesocket(ConnectSocket);
		WSACleanup();
		return false;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return true;
}
