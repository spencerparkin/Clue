#include "Card.h"

using namespace Clue;

bool Accusation::operator==(const Accusation& accusation) const
{
	if (this->room != accusation.room)
		return false;

	if (this->weapon != accusation.weapon)
		return false;

	if (this->character != accusation.character)
		return false;

	return true;
}

Card::Card()
{
	this->value = -1;
	this->type = Type::RoomType;
}

Card::Card(int value, Type type)
{
	this->value = value;
	this->type = type;
}

Card::Card(const Card& card)
{
	this->value = card.value;
	this->type = card.type;
}

bool Card::operator==(const Card& card) const
{
	return this->value == card.value && this->type == card.type;
}

bool Card::operator<(const Card& card) const
{
	if (this->type == card.type)
		return this->value < card.value;

	return this->type < card.type;
}

/*static*/ void Card::GenerateDeck(std::vector<Card>& cardArray)
{
	cardArray.clear();

	cardArray.push_back(Card({ Weapon::Candelstick, Type::WeaponType }));
	cardArray.push_back(Card({ Weapon::Dagger, Type::WeaponType }));
	cardArray.push_back(Card({ Weapon::LeadPipe, Type::WeaponType }));
	cardArray.push_back(Card({ Weapon::Revolver, Type::WeaponType }));
	cardArray.push_back(Card({ Weapon::Rope, Type::WeaponType }));
	cardArray.push_back(Card({ Weapon::Wrench, Type::WeaponType }));

	cardArray.push_back(Card({ Character::ColonelMustard, Type::CharacterType }));
	cardArray.push_back(Card({ Character::MissScarlett, Type::CharacterType }));
	cardArray.push_back(Card({ Character::MrsPeacock, Type::CharacterType }));
	cardArray.push_back(Card({ Character::MrsWhite, Type::CharacterType }));
	cardArray.push_back(Card({ Character::ProfessorPlum, Type::CharacterType }));
	cardArray.push_back(Card({ Character::ReverendGreen, Type::CharacterType }));

	cardArray.push_back(Card({ Room::BallRoom, Type::RoomType }));
	cardArray.push_back(Card({ Room::BilliardRoom, Type::RoomType }));
	cardArray.push_back(Card({ Room::Conservatory, Type::RoomType }));
	cardArray.push_back(Card({ Room::DiningRoom, Type::RoomType }));
	cardArray.push_back(Card({ Room::Hall, Type::RoomType }));
	cardArray.push_back(Card({ Room::Kitchen, Type::RoomType }));
	cardArray.push_back(Card({ Room::Library, Type::RoomType }));
	cardArray.push_back(Card({ Room::Lounge, Type::RoomType }));
	cardArray.push_back(Card({ Room::Study, Type::RoomType }));
}

/*static*/ void Card::ShuffleDeck(std::vector<Card>& cardArray)
{
	for (int i = 0; i < (int)cardArray.size() - 1; i++)
	{
		int j = RandomInteger(i, int(cardArray.size() - 1));
		if (i == j)
			continue;

		Card card = cardArray[i];
		cardArray[i] = cardArray[j];
		cardArray[j] = card;
	}
}

/*static*/ int Card::RandomInteger(int min, int max)
{
	int i = int(::round(double(min) + (double(std::rand()) / double(RAND_MAX)) * double(max - min)));
	if (i < min)
		i = min;
	if (i > max)
		i = max;
	return i;
}

/*static*/ bool Card::FindAndRemoveCardOfType(std::vector<Card>& cardArray, Card::Type cardType, Card& removedCard)
{
	for (int i = 0; i < (int)cardArray.size(); i++)
	{
		const Card& card = cardArray[i];

		if (card.type == cardType)
		{
			removedCard = card;
			cardArray.erase(cardArray.begin() + i);
			return true;
		}
	}

	return true;
}

bool CardCompare::operator()(const Card& cardA, const Card& cardB) const
{
	return cardA < cardB;
}

std::string Card::GetLabel() const
{
	switch (this->type)
	{
	case Type::CharacterType:
		return GetCharacterName(this->character);
	case Type::WeaponType:
		return GetWeaponName(this->weapon);
	case Type::RoomType:
		return GetRoomName(this->room);
	}

	return "?";
}

/*static*/ void Card::GetCharacterColor(Character character, double& r, double& g, double& b)
{
	r = 0.0;
	g = 0.0;
	b = 0.0;

	switch (character)
	{
		case Character::MissScarlett:
		{
			r = 1.0;
			g = 0.0;
			b = 0.0;
			break;
		}
		case Character::ColonelMustard:
		{
			r = 1.0;
			g = 1.0;
			b = 0.0;
			break;
		}
		case Character::MrsWhite:
		{
			r = 1.0;
			g = 1.0;
			b = 1.0;
			break;
		}
		case Character::ReverendGreen:
		{
			r = 0.0;
			g = 1.0;
			b = 0.0;
			break;
		}
		case Character::MrsPeacock:
		{
			r = 0.0;
			g = 0.0;
			b = 1.0;
			break;
		}
		case Character::ProfessorPlum:
		{
			r = 1.0;
			g = 0.0;
			b = 1.0;
			break;
		}
	}
}

/*static*/ std::string Card::GetCharacterName(Character character)
{
	std::string name;

	switch (character)
	{
	case Character::MissScarlett:
		name = "Miss Scarlett";
		break;
	case Character::ColonelMustard:
		name = "Colonel Mustard";
		break;
	case Character::MrsWhite:
		name = "Mrs. White";
		break;
	case Character::ReverendGreen:
		name = "Rev. Green";
		break;
	case Character::MrsPeacock:
		name = "Mrs. Peacock";
		break;
	case Character::ProfessorPlum:
		name = "Prof. Plum";
		break;
	}

	return name;
}

/*static*/ std::string Card::GetWeaponName(Weapon weapon)
{
	std::string name;

	switch (weapon)
	{
	case Weapon::Candelstick:
		name = "Candelstick";
		break;
	case Weapon::Dagger:
		name = "Dagger";
		break;
	case Weapon::LeadPipe:
		name = "Lead Pipe";
		break;
	case Weapon::Revolver:
		name = "Revolver";
		break;
	case Weapon::Rope:
		name = "Rope";
		break;
	case Weapon::Wrench:
		name = "Wrench";
		break;
	}

	return name;
}

/*static*/ std::string Card::GetRoomName(Room room)
{
	std::string name;

	switch (room)
	{
	case Room::BallRoom:
		name = "Ball Room";
		break;
	case Room::BilliardRoom:
		name = "Billiard Room";
		break;
	case Room::Conservatory:
		name = "Conservatory";
		break;
	case Room::DiningRoom:
		name = "Dining Room";
		break;
	case Room::Hall:
		name = "Hall";
		break;
	case Room::Kitchen:
		name = "Kitchen";
		break;
	case Room::Library:
		name = "Library";
		break;
	case Room::Lounge:
		name = "Lounge";
		break;
	case Room::Study:
		name = "Study";
		break;
	}

	return name;
}