#pragma once
#include "ServerNetwork.h"
#include "NetworkData.h"
#include <iostream>

class ServerGame
{

public:
	ServerGame(void);
	~ServerGame(void);

	void update();

	void ReceiveFromClients();

	void SendActionPackets();
	void SendChatPackets(string message);

private:
	static unsigned int m_ClientID;

	ServerNetwork* m_Network;

	// data buffer
	char m_NetworkData[MAX_PACKET_SIZE];
};










