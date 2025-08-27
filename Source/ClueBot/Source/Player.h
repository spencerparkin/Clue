#pragma once

#include "Card.h"
#include "PacketThread.h"
#include "BoardGraph.h"
#include <string>
#include <memory>
#include <optional>

class PacketHandler;

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

	struct GameData
	{
		std::vector<Clue::Card> cardArray;
		Clue::Character character;
		Clue::BoardGraph boardGraph;
		std::optional<Clue::Room> roomTarget;
		std::map<Clue::Character, std::shared_ptr<Clue::BoardGraph::Node>> nodeOccupiedMap;
	};

	GameData* GetGameData();
	Clue::PacketThread* GetPacketThread();

protected:
	virtual bool ProcessPacket(const std::shared_ptr<Clue::Packet> packet);

	GameData gameData;
	std::string ipAddr;
	int port;
	std::shared_ptr<Clue::PacketThread> packetThread;
	volatile bool keepRunning;

private:
	std::map<uint32_t, std::shared_ptr<PacketHandler>> packetHandlerMap;
};