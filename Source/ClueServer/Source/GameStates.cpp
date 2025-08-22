#include "GameStates.h"
#include "Packet.h"
#include <assert.h>

using namespace Clue;

//---------------------------------------- SetupGameState ----------------------------------------

SetupGameState::SetupGameState()
{
}

/*virtual*/ SetupGameState::~SetupGameState()
{
}

/*virtual*/ GameState::Result SetupGameState::Run(Server::GameData* gameData, std::shared_ptr<GameState>& nextState)
{
	gameData->whoseTurn = 0;
	gameData->presentDiceRoll = 0;

	gameData->boardGraph.Regenerate();

	std::vector<Card> cardArray;
	Card::GenerateDeck(cardArray);
	Card::ShuffleDeck(cardArray);

	Card roomCard, weaponCard, characterCard;
	Card::FindAndRemoveCardOfType(cardArray, Card::Type::RoomType, roomCard);
	Card::FindAndRemoveCardOfType(cardArray, Card::Type::WeaponType, weaponCard);
	Card::FindAndRemoveCardOfType(cardArray, Card::Type::CharacterType, characterCard);

	gameData->correctAccusation.room = roomCard.room;
	gameData->correctAccusation.weapon = weaponCard.weapon;
	gameData->correctAccusation.character = characterCard.character;

	assert(gameData->playerArray.size() <= CLUE_NUM_CHARACTERS);

	// Assign a character to each player.  I might do this randomly,
	// but for now, just do it like this.  Also, place them on the
	// game board at their starting locations.
	for (int i = 0; i < (int)gameData->playerArray.size(); i++)
	{
		Player* player = gameData->playerArray[i].get();
		player->character = Character(i);
		player->nodeOccupied = gameData->boardGraph.GetCharacterStartLocation(player->character);
	}

	// Deal cards to our local player representatives or proxies, if you will.
	while (cardArray.size() > 0)
	{
		for (int i = 0; i < (int)gameData->playerArray.size(); i++)
		{
			Player* player = gameData->playerArray[i].get();
			Card card = cardArray.back();
			cardArray.pop_back();
			player->cardArray.push_back(card);
		}
	}

	// Now deal the cards to the associated remote player clients.
	for (int i = 0; i < (int)gameData->playerArray.size(); i++)
	{
		Player* player = gameData->playerArray[i].get();

		std::shared_ptr<StructurePacket<CharacterAndCards>> packet = std::make_shared<StructurePacket<CharacterAndCards>>(CLUE_PACKET_TYPE_CHAR_AND_CARDS);
		
		packet->data.character = player->character;
		packet->data.id = player->nodeOccupied->GetId();
		packet->data.numCards = (int)player->cardArray.size();
		for (int j = 0; j < (int)player->cardArray.size(); j++)
			packet->data.cardArray[j] = player->cardArray[j];

		player->packetThread.SendPacket(packet);
	}

	// Introduce the players to one another.  Each player needs to know
	// their fellow players, what character they are, and how many cards.
	// Of course, what cards they have is kept a secret known only them.
	for (int i = 0; i < (int)gameData->playerArray.size(); i++)
	{
		Player* playerA = gameData->playerArray[i].get();

		for (int j = 0; j < (int)gameData->playerArray.size(); j++)
		{
			if (i == j)
				continue;

			Player* playerB = gameData->playerArray[j].get();

			std::shared_ptr<StructurePacket<PlayerIntroduction>> packet = std::make_shared<StructurePacket<PlayerIntroduction>>(CLUE_PACKET_TYPE_PLAYER_INTRO);

			packet->data.character = playerB->character;
			packet->data.numCards = (int)playerB->cardArray.size();

			playerA->packetThread.SendPacket(packet);
		}
	}

	nextState = std::make_shared<RollForPlayerState>();
	return Result::LeaveState;
}

//---------------------------------------- RollForPlayerState ----------------------------------------

RollForPlayerState::RollForPlayerState()
{
}

/*virtual*/ RollForPlayerState::~RollForPlayerState()
{
}

/*virtual*/ GameState::Result RollForPlayerState::Run(Server::GameData* gameData, std::shared_ptr<GameState>& nextState)
{
	gameData->presentDiceRoll = Card::RandomInteger(1, 6) + Card::RandomInteger(1, 6);

	Player* currentPlayer = gameData->playerArray[gameData->whoseTurn].get();

	std::shared_ptr<StructurePacket<DiceRoll>> packet = std::make_shared<StructurePacket<DiceRoll>>(CLUE_PACKET_TYPE_DICE_ROLL);
	packet->data.rollAmount = gameData->presentDiceRoll;
	currentPlayer->packetThread.SendPacket(packet);

	nextState = std::make_shared<WaitForPlayerTravelState>();
	return Result::LeaveState;
}

//---------------------------------------- WaitForPlayerTravelState ----------------------------------------

WaitForPlayerTravelState::WaitForPlayerTravelState()
{
}

/*virtual*/ WaitForPlayerTravelState::~WaitForPlayerTravelState()
{
}

/*virtual*/ GameState::Result WaitForPlayerTravelState::Run(Server::GameData* gameData, std::shared_ptr<GameState>& nextState)
{
	Player* currentPlayer = gameData->playerArray[gameData->whoseTurn].get();

	std::shared_ptr<Packet> packet;
	if (!currentPlayer->packetThread.ReceivePacket(packet))
		return Result::ContinueState;

	auto playerTravel = dynamic_cast<PlayerTravelRequested*>(packet.get());
	if (!playerTravel)
		return Result::ContinueState;

	std::shared_ptr<BoardGraph::Node> targetNode = gameData->boardGraph.FindNodeWithID(playerTravel->nodeId);
	if (!targetNode.get())
	{
		std::shared_ptr<StructurePacket<PlayerTravelRejected>> rejectionPacket = std::make_shared<StructurePacket<PlayerTravelRejected>>(CLUE_PACKET_TYPE_PLAYER_TRAVEL_REJECTED);
		rejectionPacket->data.type = PlayerTravelRejected::TARGET_NODE_DOESNT_EXIST;
		currentPlayer->packetThread.SendPacket(rejectionPacket);
		return Result::ContinueState;
	}

	std::vector<BoardGraph::Node*> nodeArray;
	bool pathFound = gameData->boardGraph.FindShortestPathBetweenNodes(currentPlayer->nodeOccupied.get(), targetNode.get(), nodeArray);
	assert(pathFound);

	int minDiceRollNeeded = BoardGraph::CalculatePathCost(nodeArray);
	if (gameData->presentDiceRoll < minDiceRollNeeded)
	{
		std::shared_ptr<StructurePacket<PlayerTravelRejected>> rejectionPacket = std::make_shared<StructurePacket<PlayerTravelRejected>>(CLUE_PACKET_TYPE_PLAYER_TRAVEL_REJECTED);
		rejectionPacket->data.type = PlayerTravelRejected::TARGET_NODE_TOO_FAR;
		currentPlayer->packetThread.SendPacket(rejectionPacket);
		return Result::ContinueState;
	}

	currentPlayer->nodeOccupied = targetNode;

	std::shared_ptr<StructurePacket<PlayerTravelAccepted>> travelPacket = std::make_shared<StructurePacket<PlayerTravelAccepted>>(CLUE_PACKET_TYPE_PLAYER_TRAVEL_ACCEPTED);
	travelPacket->data.character = currentPlayer->character;
	travelPacket->data.nodeId = targetNode->GetId();

	for (std::shared_ptr<Player>& player : gameData->playerArray)
		player->packetThread.SendPacket(travelPacket);

	// TODO: Here our next state depends on whether the current player landed in a room or the hallway.

	// TODO: We must enforce that any accustaion made is only done in the room where the accuser is located,
	//       and an accuser can only make an accusation if they are in a room.  Also, an accusation will
	//       pull the character in question into that room.  This must be communicated to all clients.

	return Result::HaltMachine;
}