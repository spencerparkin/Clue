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

	Clue::Character character;
	std::vector<Clue::Card> cardArray;
	std::shared_ptr<Clue::BoardGraph::Node> nodeOccupied;
	Clue::PacketThread packetThread;

	// TODO: We must track whether an accusation was made in the present room, if any,
	//       on the last turn of the player, because you must leave that room before
	//       you can make another accusation in it.  We can forgo this rule for the
	//       first implementation to keep things simple, then enforce it later.
};