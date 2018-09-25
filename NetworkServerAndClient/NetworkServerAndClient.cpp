// may need #include "stdafx.h" in visual studio
#include "stdafx.h"
#include "ServerGame.h"
#include "ClientGame.h"

#include <process.h>

#include <iostream>
#include <string>
#include <sstream> 

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

void serverLoop(void *);
void clientLoop(string command);
void StartActionLoop(ServerGame* server);

ServerGame * server;
ClientGame * client;

int main()
{
	server = new ServerGame();

	// create thread for the server to constantly be able to respond
	_beginthread(serverLoop, 0, (void*)12);

	client = new ClientGame();

	while (true)
	{
		string line;
		std::getline(cin, line);

		clientLoop(line);
	}
}

void serverLoop(void * arg)
{
	while (true)
	{
		server->update();
	}
}

void clientLoop(string command)
{
	int it = 0;

	if (command == "start")
	{
		cout << "client update initiated" << endl;
		while (it < 10)
		{
			//do game stuff

			client->Update();
			++it;
		}
	}
	else if (command == "action")
	{
		StartActionLoop(server);

		client->Update();		
	}
	else if (command.substr(0, 2) == "s:")
	{
		cout << "Message sent" << endl;
		server->SendChatPackets(command.substr(2));

		client->Update();
	}
	else if (command.substr(0, 2) == "c:")
	{
		cout << "Message sent" << endl;
		client->SendChatPackets(command.substr(2));

		client->Update();
	}
	else if (command == "close")
	{
		client->CloseConnections();
	}
}

void StartActionLoop(ServerGame* server)
{
	server->SendActionPackets();
}










