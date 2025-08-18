#pragma once

#include "PacketThread.h"

/**
 * 
 */
class Player
{
public:
	Player(SOCKET connectedSocket);
	virtual ~Player();

	bool Setup();
	bool Shutdown();

	Clue::PacketThread packetThread;
};