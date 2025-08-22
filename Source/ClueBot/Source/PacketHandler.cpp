#include "PacketHandler.h"
#include "Player.h"

using namespace Clue;

//------------------------------------------ CharAndCardsPacketHandler ------------------------------------------

/*virtual*/ bool CharAndCardsPacketHandler::HandlePacket(const std::shared_ptr<Packet> packet, Player* player)
{
	auto* structurePacket = dynamic_cast<const StructurePacket<CharacterAndCards>*>(packet.get());
	if (!structurePacket)
		return false;

	Player::GameData* gameData = player->GetGameData();
	gameData->boardGraph.Regenerate();
	gameData->character = structurePacket->data.character;
	gameData->cardArray.clear();
	for (uint32_t i = 0; i < structurePacket->data.numCards; i++)
		gameData->cardArray.push_back(structurePacket->data.cardArray[i]);

	gameData->nodeOccupied = gameData->boardGraph.FindNodeWithID(structurePacket->data.id);
	if (!gameData->nodeOccupied.get())
		return false;

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
		// TODO: Here we would try to choose a room intelligently.  For now, just choose a random room.

		//...
	}
	else
	{
		// TODO: Re-evaluate our current choice of room target.  Do we still want to go there?
	}

	// TODO: Query board graph for any node in a given room, then use that to calculate a path.
	//gameData->boardGraph.FindShortestPathBetweenNodes(gameData->nodeOccupied.get(), )
	// TODO: Figure out how far along the path we can go in the allotted roll.

	//structurePacket->data.rollAmount;

	return true;
}