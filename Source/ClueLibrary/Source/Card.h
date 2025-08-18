#pragma once

#include "Defines.h"
#include <vector>
#include <map>
#include <string>

namespace Clue
{
	enum Room : int
	{
		Kitchen,
		DiningRoom,
		Lounge,
		BallRoom,
		Hall,
		Conservatory,
		BilliardRoom,
		Library,
		Study
	};

	enum Weapon : int
	{
		Candelstick,
		Dagger,
		LeadPipe,
		Revolver,
		Rope,
		Wrench
	};

	enum Character : int
	{
		MissScarlett,
		ColonelMustard,
		MrsWhite,
		ReverendGreen,
		MrsPeacock,
		ProfessorPlum
	};

	struct Accusation
	{
		Weapon weapon;
		Character character;
		Room room;

		bool operator==(const Accusation& accustaion) const;
	};

	class CLUE_LIBRARY_API Card
	{
	public:

		enum Type
		{
			RoomType,
			WeaponType,
			CharacterType
		};

		Card();
		Card(int value, Type type);
		Card(const Card& card);

		bool operator==(const Card& card) const;
		bool operator<(const Card& card) const;

		std::string GetLabel() const;

		union
		{
			Room room;
			Weapon weapon;
			Character character;
			int value;
		};

		Type type;

		static void GenerateDeck(std::vector<Card>& cardArray);
		static void ShuffleDeck(std::vector<Card>& cardArray);
		static bool FindAndRemoveCardOfType(std::vector<Card>& cardArray, Card::Type cardType, Card& removedCard);
		static int RandomInteger(int min, int max);
		static void GetCharacterColor(Character character, double& r, double& g, double& b);
		static std::string GetCharacterName(Character character);
		static std::string GetWeaponName(Weapon weapon);
		static std::string GetRoomName(Room room);
	};

	struct CardCompare
	{
		bool operator()(const Card& cardA, const Card& cardB) const;
	};

	template<typename V>
	class CardMap : public std::map<Card, V, CardCompare>
	{
	public:
	};
}