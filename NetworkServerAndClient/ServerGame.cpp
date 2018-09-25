#include "stdafx.h" 
#include "ServerGame.h"

unsigned int ServerGame::m_ClientID;

ServerGame::ServerGame(void)
{
	// id's to assign clients for our table
	m_ClientID = 0;

	// set up the server network to listen 
	m_Network = new ServerNetwork();
}

void ServerGame::update()
{
	// get new clients
	if (m_Network->AcceptNewClient(m_ClientID))
	{
		printf("client %d has been connected to the server\n", m_ClientID);

		m_ClientID++;
	}

	ReceiveFromClients();
}

void ServerGame::ReceiveFromClients()
{

	Packet packet;

	// go through all clients

	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = m_Network->m_Sessions.begin(); iter != m_Network->m_Sessions.end(); iter++)
	{
		int data_length = m_Network->ReceiveData(iter->first, m_NetworkData);

		if (data_length <= 0)
		{
			//no data recieved

			continue;
		}

		int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(m_NetworkData[i]));
			i += sizeof(Packet);

			switch (packet.packet_type) {

			case INIT_CONNECTION:

				printf("server received init packet from client\n");

				//SendActionPackets();

				break;

			case ACTION_EVENT:

				printf("server received action event packet from client\n");

				//SendActionPackets();

				break;
			case CHAT_EVENT:

				printf("server received chat event packet from client\n");
				printf("Client: ");
				std::cout << packet.msg << std::endl;

				break;

			default:

				printf("error in packet types\n");

				break;
			}
		}
	}
}

void ServerGame::SendActionPackets()
{
	// send action packet

	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);

	m_Network->SendToAll(packet_data, packet_size);
}

void ServerGame::SendChatPackets(string message)
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = CHAT_EVENT;
	strcpy_s(packet.msg, message.c_str());

	packet.serialize(packet_data);

	m_Network->SendToAll(packet_data, packet_size);
}

