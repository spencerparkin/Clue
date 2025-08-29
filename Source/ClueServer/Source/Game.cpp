#include "Game.h"
#include "Player.h"
#include "Card.h"
#include "Server.h"
#include "Packet.h"
#include <utility>
#include <assert.h>

using namespace Clue;

//-------------------------------------- GameTask --------------------------------------

GameTask::GameTask(GameTask&& gameTask) : coroHandle(std::exchange(gameTask.coroHandle, nullptr))
{
}

GameTask::GameTask(promise_type* promiseType) : coroHandle(CoroHandle::from_promise(*promiseType))
{
}

/*virtual*/ GameTask::~GameTask()
{
	if (this->coroHandle)
		this->coroHandle.destroy();
}

bool GameTask::IsDone()
{
	return this->coroHandle.done();
}

void GameTask::Resume()
{
	this->coroHandle.resume();
}

//-------------------------------------- GameTask::promise_type --------------------------------------

GameTask  GameTask::promise_type::get_return_object()
{
	return GameTask(this);
}

std::suspend_always GameTask::promise_type::initial_suspend() noexcept
{
	return std::suspend_always();
}

std::suspend_always GameTask::promise_type::final_suspend() noexcept
{
	return std::suspend_always();
}

std::suspend_always GameTask::promise_type::yield_value(int) noexcept
{
	return std::suspend_always();
}

void GameTask::promise_type::return_void() noexcept
{
}

void GameTask::promise_type::unhandled_exception() noexcept
{
}

//-------------------------------------- PlayGame --------------------------------------

GameTask PlayGame(Server* server)
{
	int whoseTurn = 0;
	int presentDiceRoll = 0;

	Clue::BoardGraph boardGraph;
	boardGraph.Regenerate();

	std::vector<Card> cardArray;
	Card::GenerateDeck(cardArray);
	Card::ShuffleDeck(cardArray);

	Card roomCard, weaponCard, characterCard;
	Card::FindAndRemoveCardOfType(cardArray, Card::Type::RoomType, roomCard);
	Card::FindAndRemoveCardOfType(cardArray, Card::Type::WeaponType, weaponCard);
	Card::FindAndRemoveCardOfType(cardArray, Card::Type::CharacterType, characterCard);

	Clue::Accusation correctAccusation;
	correctAccusation.room = roomCard.room;
	correctAccusation.weapon = weaponCard.weapon;
	correctAccusation.character = characterCard.character;

	const std::vector<std::shared_ptr<Player>>& playerArray = server->GetPlayerArray();
	assert(playerArray.size() <= CLUE_NUM_CHARACTERS);

	// Assign a character to each player.  I might do this randomly,
	// but for now, just do it like this.  Also, place them on the
	// game board at their starting locations.
	for (int i = 0; i < (int)playerArray.size(); i++)
	{
		Player* player = playerArray[i].get();
		player->character = Character(i);
		player->nodeOccupied = boardGraph.GetCharacterStartLocation(player->character);
	}

	// Let all players know where all players are located.
	for (int i = 0; i < (int)playerArray.size(); i++)
	{
		Player* playerA = playerArray[i].get();

		std::shared_ptr<StructurePacket<PlayerTravelAccepted>> travelPacket = std::make_shared<StructurePacket<PlayerTravelAccepted>>();
		travelPacket->data.character = playerA->character;
		travelPacket->data.nodeId = playerA->nodeOccupied->GetId();

		for (int j = 0; j < (int)playerArray.size(); j++)
		{
			Player* playerB = playerArray[j].get();

			playerB->packetThread.SendPacket(travelPacket);
		}
	}

	// Deal cards to all the players.
	while (cardArray.size() > 0)
	{
		for (int i = 0; i < (int)playerArray.size(); i++)
		{
			Player* player = playerArray[i].get();
			Card card = cardArray.back();
			cardArray.pop_back();
			player->cardArray.push_back(card);
		}
	}

	// Now deal the cards to the associated remote player clients.
	for (int i = 0; i < (int)playerArray.size(); i++)
	{
		Player* player = playerArray[i].get();

		std::shared_ptr<StructurePacket<CharacterAndCards>> charCardsPacket = std::make_shared<StructurePacket<CharacterAndCards>>();

		charCardsPacket->data.character = player->character;
		charCardsPacket->data.numCards = (int)player->cardArray.size();
		for (int j = 0; j < (int)player->cardArray.size(); j++)
			charCardsPacket->data.cardArray[j] = player->cardArray[j];

		player->packetThread.SendPacket(charCardsPacket);
	}

	// Introduce the players to one another.  Each player needs to know
	// their fellow players, what character they are, and how many cards.
	// Of course, what cards they have is kept a secret known only them.
	for (int i = 0; i < (int)playerArray.size(); i++)
	{
		Player* playerA = playerArray[i].get();

		for (int j = 0; j < (int)playerArray.size(); j++)
		{
			if (i == j)
				continue;

			Player* playerB = playerArray[j].get();

			std::shared_ptr<StructurePacket<PlayerIntroduction>> packet = std::make_shared<StructurePacket<PlayerIntroduction>>();

			packet->data.character = playerB->character;
			packet->data.numCards = (int)playerB->cardArray.size();

			playerA->packetThread.SendPacket(packet);
		}
	}

	// We now enter the main game loop.
	while (true)
	{
		// Roll for the current player.
		presentDiceRoll = Card::RandomInteger(1, 6) + Card::RandomInteger(1, 6);

		// Tell the current player what their dice roll is.  This also prompts them to request a board travel.
		Player* currentPlayer = playerArray[whoseTurn].get();
		std::shared_ptr<StructurePacket<DiceRoll>> diceRollPacket = std::make_shared<StructurePacket<DiceRoll>>();
		diceRollPacket->data.rollAmount = presentDiceRoll;
		currentPlayer->packetThread.SendPacket(diceRollPacket);

		// Persist in getting a valid travel packet from the current player.
		std::shared_ptr<BoardGraph::Node> targetNode;
		while (true)
		{
			std::shared_ptr<Packet> playerTravelPacket;
			while (!currentPlayer->packetThread.ReceivePacket(playerTravelPacket))
				co_yield 0;

			auto playerTravel = dynamic_cast<StructurePacket<PlayerTravelRequested>*>(playerTravelPacket.get());
			if (!playerTravel)
				continue;

			targetNode = boardGraph.FindNodeWithID(playerTravel->data.nodeId);
			if (!targetNode.get())
			{
				std::shared_ptr<StructurePacket<PlayerTravelRejected>> rejectionPacket = std::make_shared<StructurePacket<PlayerTravelRejected>>();
				rejectionPacket->data.type = PlayerTravelRejected::TARGET_NODE_DOESNT_EXIST;
				currentPlayer->packetThread.SendPacket(rejectionPacket);
				continue;
			}

			std::vector<BoardGraph::Node*> nodeArray;
			bool pathFound = boardGraph.FindShortestPathBetweenNodes(currentPlayer->nodeOccupied.get(), targetNode.get(), nodeArray);
			assert(pathFound);

			int minDiceRollNeeded = BoardGraph::CalculatePathCost(nodeArray);
			if (presentDiceRoll < minDiceRollNeeded)
			{
				std::shared_ptr<StructurePacket<PlayerTravelRejected>> rejectionPacket = std::make_shared<StructurePacket<PlayerTravelRejected>>();
				rejectionPacket->data.type = PlayerTravelRejected::TARGET_NODE_TOO_FAR;
				currentPlayer->packetThread.SendPacket(rejectionPacket);
				continue;
			}

			bool alreadyOccupied = false;
			for (std::shared_ptr<Player> player : playerArray)
			{
				if (player.get() != currentPlayer && player->nodeOccupied == targetNode)
				{
					std::shared_ptr<StructurePacket<PlayerTravelRejected>> rejectionPacket = std::make_shared<StructurePacket<PlayerTravelRejected>>();
					rejectionPacket->data.type = PlayerTravelRejected::TARGET_NODE_ALREADY_OCCUPIED;
					currentPlayer->packetThread.SendPacket(rejectionPacket);
					alreadyOccupied = true;
					break;
				}
			}

			if (alreadyOccupied)
				continue;

			break;
		}

		// Move the player to the new location.
		currentPlayer->nodeOccupied = targetNode;

		std::shared_ptr<StructurePacket<PlayerTravelAccepted>> travelPacket = std::make_shared<StructurePacket<PlayerTravelAccepted>>();
		travelPacket->data.character = currentPlayer->character;
		travelPacket->data.nodeId = targetNode->GetId();

		for (std::shared_ptr<Player> player : playerArray)
			player->packetThread.SendPacket(travelPacket);

		// TODO: Write logic for making accusations, refuting them, etc., here.

		// TODO: If the player landed in a room then they have the option of making an accusation.

		// TODO: We must enforce that any accustaion made is only done in the room where the accuser is located,
		//       and an accuser can only make an accusation if they are in a room.  Also, an accusation will
		//       pull the character in question into that room.  This must be communicated to all clients.

		// Finally, end the player's turn and loop around to begin the next player's turn.
		whoseTurn = (whoseTurn + 1) % playerArray.size();
	}
}