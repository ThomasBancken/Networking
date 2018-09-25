#pragma once
#include <string>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {
	INIT_CONNECTION = 0,
	ACTION_EVENT = 1,
	CHAT_EVENT = 2
};

struct Packet {
	unsigned int packet_type;
	char msg[512];

	void serialize(char * data) {
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Packet));
	}
};



