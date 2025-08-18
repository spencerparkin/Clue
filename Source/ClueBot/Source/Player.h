#pragma once

#include "Card.h"
#include "PacketThread.h"
#include <string>
#include <memory>

/**
 * A Clue player is very simple.  It just sits around and responds to
 * inqueries and requests from the server.  In some cases, information
 * from the server is simply noted, and nothing more.
 */
class Player : public Clue::Thread
{
public:
	Player(const std::string& ipAddr, int port);
	virtual ~Player();

	virtual bool Join() override;
	virtual void Run() override;

protected:
	virtual bool ProcessPacket(std::shared_ptr<Clue::Packet> packet);

	std::vector<Clue::Card> cardArray;
	std::string ipAddr;
	int port;
	std::shared_ptr<Clue::PacketThread> packetThread;
	volatile bool keepRunning;
};