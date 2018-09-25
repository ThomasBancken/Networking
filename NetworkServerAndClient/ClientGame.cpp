#include "stdafx.h"
#include "ClientGame.h" 

ClientGame::ClientGame(void)
{
	m_Network = new ClientNetwork();

	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_CONNECTION;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(m_Network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::SendActionPackets()
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(m_Network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::SendChatPackets(std::string message)
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = CHAT_EVENT;
	strcpy_s(packet.msg, message.c_str());

	packet.serialize(packet_data);

	NetworkServices::sendMessage(m_Network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::Update()
{
	Packet packet;
	int data_length = m_Network->ReceivePackets(m_NetworkData);

	if (data_length <= 0)
	{
		//no data recieved

		return;
	}

	int i = 0;
	while (i < (unsigned int)data_length)
	{
		packet.deserialize(&(m_NetworkData[i]));
		i += sizeof(Packet);

		switch (packet.packet_type) {

		case ACTION_EVENT:

			printf("client received action event packet from server\n");

			SendActionPackets();

			break;
		case CHAT_EVENT:

			printf("client received chat event packet from server\n");
			printf("Server: ");
			std::cout << packet.msg << std::endl;

			break;

		default:

			printf("error in packet types\n");

			break;
		}
	}
}

void ClientGame::CloseConnections()
{
	m_Network->CloseConnection();
}