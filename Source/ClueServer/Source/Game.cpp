#include "Game.h"
#include "Player.h"
#include "Card.h"
#include "Server.h"
#include "Packet.h"
#include <utility>
#include <assert.h>

using namespace Clue;

//-------------------------------------- GameTask --------------------------------------

GameTask::GameTask(GameTask&& gameTask) noexcept : coroHandle(std::exchange(gameTask.coroHandle, nullptr))
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

bool GameTask::GotPacketFromPlayer()
{
	return this->coroHandle.promise().GotPacketFromPlayer();
}

//-------------------------------------- GameTask::promise_type --------------------------------------

GameTask::promise_type::promise_type()
{
	this->packetNeededFromPlayer = nullptr;
}

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

GameTask::PacketAwaiter GameTask::promise_type::await_transform(Player* packetNeededFromPlayer) noexcept
{
	this->packetNeededFromPlayer = packetNeededFromPlayer;
	return PacketAwaiter(this);
}

void GameTask::promise_type::return_void() noexcept
{
}

void GameTask::promise_type::unhandled_exception() noexcept
{
}

std::shared_ptr<Clue::Packet> GameTask::promise_type::GetPlayerPacket()
{
	return this->playerPacket;
}

bool GameTask::promise_type::GotPacketFromPlayer()
{
	if (!this->packetNeededFromPlayer)
		return false;

	return this->packetNeededFromPlayer->packetThread.ReceivePacket(this->playerPacket);
}

//-------------------------------------- GameTask::PacketAwaiter --------------------------------------

GameTask::PacketAwaiter::PacketAwaiter(promise_type* promise)
{
	this->promise = promise;
}

bool GameTask::PacketAwaiter::await_ready() const noexcept
{
	return false;
}

std::shared_ptr<Packet> GameTask::PacketAwaiter::await_resume() const noexcept
{
	return this->promise->GetPlayerPacket();
}

void GameTask::PacketAwaiter::await_suspend(std::coroutine_handle<promise_type> coroHandle) const noexcept
{
}

//-------------------------------------- PlayGame --------------------------------------

GameTask PlayGame(Server* server)
{
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
	int whoseTurn = 0;
	while (true)
	{
		// TODO: If all players are disqualified, end the game.  I suppose that could happen if everyone is just gets it wrong.

		Player* currentPlayer = playerArray[whoseTurn].get();
		if (!currentPlayer->disqualified)
		{
			// Roll for the current player.
			int presentDiceRoll = Card::RandomInteger(1, 6) + Card::RandomInteger(1, 6);

			// Tell the current player what their dice roll is.  This also prompts them to request a board travel.
			std::shared_ptr<StructurePacket<DiceRoll>> diceRollPacket = std::make_shared<StructurePacket<DiceRoll>>();
			diceRollPacket->data.rollAmount = presentDiceRoll;
			currentPlayer->packetThread.SendPacket(diceRollPacket);

			// Persist in getting a valid travel packet from the current player.
			std::shared_ptr<BoardGraph::Node> targetNode;
			while (true)
			{
				std::shared_ptr<Packet> playerTravelPacket = co_await currentPlayer;

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

			// TODO: There is a rule that you can't make an accusation in the same room on consecutive turns.
			//       I'm just going to ignore that for now, but will revisit it later.
			if (currentPlayer->nodeOccupied->IsRoom())
			{
				auto playerCanAccusePacket = std::make_shared<StructurePacket<PlayerCanMakeAccusation>>();
				playerCanAccusePacket->data.room = currentPlayer->nodeOccupied->GetRoom().value();
				currentPlayer->packetThread.SendPacket(playerCanAccusePacket);

				// Persist in getting a valid accustaion from the player.
				Accusation currentAccusation;
				bool pass = false;
				bool isFinalAccusation = false;
				while (true)
				{
					std::shared_ptr<Packet> playerAccusationPacket = co_await currentPlayer;

					auto playerAccusation = dynamic_cast<StructurePacket<PlayerMakesAccusation>*>(playerAccusationPacket.get());
					if (!playerAccusation)
						continue;

					if (playerAccusation->data.accusation.room != currentPlayer->nodeOccupied->GetRoom().value())
					{
						auto rejectionPacket = std::make_shared<StructurePacket<PlayerAccusationRejected>>();
						rejectionPacket->data.type = PlayerAccusationRejected::INVALID_ROOM;
						currentPlayer->packetThread.SendPacket(rejectionPacket);
						continue;
					}

					currentAccusation = playerAccusation->data.accusation;
					pass = (playerAccusation->data.flags & PlayerMakesAccusation::Flag::PASS) != 0;
					isFinalAccusation = (playerAccusation->data.flags & PlayerMakesAccusation::Flag::FINAL) != 0;
					break;
				}

				// A player can choose to pass on making an accusation.  They might do this if
				// they want to camp out in the room that they're in.
				if (!pass)
				{
					// Let everyone know what the current accusation is.
					for (int i = 0; i < playerArray.size(); i++)
					{
						Player* player = playerArray[i].get();
						auto presentAccusationPacket = std::make_shared<StructurePacket<PresentAccusation>>();
						presentAccusationPacket->data.accuser = currentPlayer->character;
						presentAccusationPacket->data.accusation = currentAccusation;
						player->packetThread.SendPacket(presentAccusationPacket);
					}

					// TODO: There is a rule where an accusation pulls a player into the room in question if
					//       they are the suspect in question.  I'm just going to ignore that for now, but will
					//       revisit it later.  To make it happen, locate the player and, if found, choose a
					//       free location in the room, then send a TavelAccepted packet to all players.

					if (isFinalAccusation)
					{
						auto finalAccusationResultPacket = std::make_shared<StructurePacket<FinalAccusationResult>>();
						finalAccusationResultPacket->data.accuser = currentPlayer->character;
						finalAccusationResultPacket->data.isCorrect = (correctAccusation == currentAccusation);

						for (std::shared_ptr<Player> player : playerArray)
							player->packetThread.SendPacket(finalAccusationResultPacket);

						if (correctAccusation != currentAccusation)
							currentPlayer->disqualified = true;
						else
						{
							// TODO: Probably need a bit more here.
							co_return;
						}
					}
					else
					{
						for (int i = 0; i < playerArray.size() - 1; i++)
						{
							int j = (whoseTurn + i + 1) % playerArray.size();
							Player* player = playerArray[j].get();

							auto playerMustRefutePacket = std::make_shared<StructurePacket<PlayerMustRefuteIfPossible>>();
							playerMustRefutePacket->data.accusation = currentAccusation;
							player->packetThread.SendPacket(playerMustRefutePacket);

							// Persist in getting a valid refute result.
							PlayerAccustaionRefute refute{};
							while (true)
							{
								std::shared_ptr<Packet> refutePacket = co_await player;

								auto refuteResultPacket = dynamic_cast<StructurePacket<PlayerAccustaionRefute>*>(refutePacket.get());
								if (!refuteResultPacket)
									continue;

								if (refuteResultPacket->data.cannotRefute)
								{
									if (player->CanRefute(currentAccusation))
									{
										auto refuteRejectPacket = std::make_shared<StructurePacket<PlayerRefuteRejected>>();
										refuteRejectPacket->data.type = PlayerRefuteRejected::Type::CAN_REFUTE_BUT_DIDNT;
										player->packetThread.SendPacket(refuteRejectPacket);
										continue;
									}
								}
								else
								{
									if (!player->HasCard(refuteResultPacket->data.card))
									{
										auto refuteRejectPacket = std::make_shared<StructurePacket<PlayerRefuteRejected>>();
										refuteRejectPacket->data.type = PlayerRefuteRejected::Type::CARD_NOT_OWNED;
										player->packetThread.SendPacket(refuteRejectPacket);
										continue;
									}
								}

								refute = refuteResultPacket->data;
								break;
							}

							if (refute.cannotRefute)
							{
								auto notRefutedPacket = std::make_shared<StructurePacket<AccusationCouldNotBeRefuted>>();
								notRefutedPacket->data.character = player->character;
								for (int k = 0; k < playerArray.size(); k++)
									playerArray[k]->packetThread.SendPacket(notRefutedPacket);
							}
							else
							{
								auto refutedWithCardPacket = std::make_shared<StructurePacket<AccusationRefutedWithCard>>();
								refutedWithCardPacket->data.refuter = player->character;
								refutedWithCardPacket->data.card = refute.card;
								currentPlayer->packetThread.SendPacket(refutedWithCardPacket);

								auto accusationRefutedPacket = std::make_shared<StructurePacket<AccusationRefuted>>();
								accusationRefutedPacket->data.refuter = player->character;
								for (int k = 0; k < playerArray.size(); k++)
									if (playerArray[k].get() != currentPlayer)
										playerArray[k]->packetThread.SendPacket(accusationRefutedPacket);

								// As soon as we encounter a player that could refute the accusation, we move on.
								break;
							}
						}
					}
				}
			}
		}

		// Finally, end the player's turn and loop around to begin the next player's turn.
		whoseTurn = (whoseTurn + 1) % playerArray.size();
	}
}