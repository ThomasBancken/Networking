#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"
#include <iostream>

class ClientGame
{

public:

	ClientGame();
	~ClientGame(void);

	ClientNetwork* m_Network;

	void SendActionPackets();
	void SendChatPackets(std::string message);

	char m_NetworkData[MAX_PACKET_SIZE];
	void CloseConnections();
	void Update();
};



