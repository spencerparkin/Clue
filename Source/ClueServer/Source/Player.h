#pragma once

#include "PacketThread.h"
#include "BoardGraph.h"

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

	bool HasCard(Clue::Card givenCard) const;
	bool CanRefute(const Clue::Accusation& accusation) const;

	Clue::Character character;
	std::vector<Clue::Card> cardArray;
	std::shared_ptr<Clue::BoardGraph::Node> nodeOccupied;
	Clue::PacketThread packetThread;
	bool disqualified;
};