#include "Player.h"

using namespace Clue;

Player::Player(SOCKET connectedSocket) : packetThread(connectedSocket)
{
	this->disqualified = false;
	this->character = Character::ColonelMustard;
}

/*virtual*/ Player::~Player()
{
}

bool Player::Setup()
{
	if (!this->packetThread.Split())
		return false;

	return true;
}

bool Player::Shutdown()
{
	if (!this->packetThread.Join())
		return false;

	return true;
}

bool Player::HasCard(Card givenCard) const
{
	for (Card card : this->cardArray)
		if (card == givenCard)
			return true;

	return false;
}

bool Player::CanRefute(const Clue::Accusation& accusation) const
{
	for (Card card : this->cardArray)
	{
		switch (card.type)
		{
		case Card::Type::RoomType:
			if (card.room == accusation.room)
				return true;
			break;
		case Card::Type::WeaponType:
			if (card.weapon == accusation.weapon)
				return true;
			break;
		case Card::Type::CharacterType:
			if (card.character == accusation.character)
				return true;
			break;
		}
	}

	return false;
}