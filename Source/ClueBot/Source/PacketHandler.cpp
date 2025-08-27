#include "PacketHandler.h"
#include "Player.h"
#include <assert.h>

using namespace Clue;

//------------------------------------------ CharAndCardsPacketHandler ------------------------------------------

/*virtual*/ bool CharAndCardsPacketHandler::HandlePacket(const std::shared_ptr<Packet> packet, Player* player)
{
	auto* structurePacket = dynamic_cast<const StructurePacket<CharacterAndCards>*>(packet.get());
	if (!structurePacket)
		return false;

	Player::GameData* gameData = player->GetGameData();
	gameData->character = structurePacket->data.character;
	gameData->cardArray.clear();
	for (uint32_t i = 0; i < structurePacket->data.numCards; i++)
		gameData->cardArray.push_back(structurePacket->data.cardArray[i]);

	return true;
}

//------------------------------------------ PlayerIntroPacketHandler ------------------------------------------

/*virtual*/ bool PlayerIntroPacketHandler::HandlePacket(const std::shared_ptr<Packet> packet, Player* player)
{
	return true;
}

//------------------------------------------ DiceRollPacketHandler ------------------------------------------

/*virtual*/ bool DiceRollPacketHandler::HandlePacket(const std::shared_ptr<Clue::Packet> packet, Player* player)
{
	auto* structurePacket = dynamic_cast<const StructurePacket<DiceRoll>*>(packet.get());
	if (!structurePacket)
		return false;

	Player::GameData* gameData = player->GetGameData();
	if (!gameData->roomTarget.has_value())
	{
		// TODO: Here we should try to choose a room intelligently.  For now, just choose a random room.

		const std::vector<std::shared_ptr<BoardGraph::Node>>& nodeArray = gameData->boardGraph.GetNodeArray();
		std::vector<std::shared_ptr<BoardGraph::Node>> roomArray;
		for (int i = 0; i < (int)nodeArray.size(); i++)
		{
			std::shared_ptr<BoardGraph::Node> node = nodeArray[i];
			if (node->GetRoom().has_value())
				roomArray.push_back(node);
		}

		int i = Card::RandomInteger(0, int(roomArray.size()) - 1);
		gameData->roomTarget = roomArray[i]->GetRoom();
	}
	else
	{
		// TODO: Re-evaluate our current choice of room target.  Do we still want to go there?
	}

	std::map<Character, std::shared_ptr<BoardGraph::Node>>::iterator iter = gameData->nodeOccupiedMap.find(gameData->character);
	assert(iter != gameData->nodeOccupiedMap.end());
	std::shared_ptr<BoardGraph::Node> nodeOccupied = iter->second;

	// TODO: Make sure we find a place in the room not already occupied.
	std::shared_ptr<BoardGraph::Node> nodeTarget = gameData->boardGraph.FindNodeWithRoom(gameData->roomTarget.value());
	std::vector<BoardGraph::Node*> nodeArray;
	if (!gameData->boardGraph.FindShortestPathBetweenNodes(nodeOccupied.get(), nodeTarget.get(), nodeArray))
		return false;
	
	int pathCost = gameData->boardGraph.CalculatePathCost(nodeArray);
	int i = pathCost < structurePacket->data.rollAmount ? pathCost : structurePacket->data.rollAmount;
	assert(0 <= i && i < (int)nodeArray.size());
	BoardGraph::Node* node = nodeArray[i];

	std::shared_ptr<StructurePacket<PlayerTravelRequested>> travelRequest = std::make_shared<StructurePacket<PlayerTravelRequested>>(CLUE_PACKET_TYPE_PLAYER_TRAVEL_REQUESTED);
	travelRequest->data.nodeId = node->GetId();
	if (!player->GetPacketThread()->SendPacket(travelRequest))
		return false;

	return true;
}

//------------------------------------------ TravelAcceptedHandler ------------------------------------------

/*virtual*/ bool TravelAcceptedHandler::HandlePacket(const std::shared_ptr<Clue::Packet> packet, Player* player)
{
	auto* structurePacket = dynamic_cast<const StructurePacket<PlayerTravelAccepted>*>(packet.get());
	if (!structurePacket)
		return false;

	Player::GameData* gameData = player->GetGameData();

	std::shared_ptr<BoardGraph::Node> node = gameData->boardGraph.FindNodeWithID(structurePacket->data.nodeId);
	if (!node.get())
		return false;

	gameData->nodeOccupiedMap.insert(std::pair(structurePacket->data.character, node));

	return true;
}