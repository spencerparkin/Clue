#pragma once

#include "Defines.h"
#include "Card.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdint.h>
#include <memory>
#include <string>

#define CLUE_PACKET_VERSION							0x00000001
#define CLUE_PACKET_MAGIC							0xDEADBEEF

namespace Clue
{
	/**
	 * 
	 */
	class CLUE_LIBRARY_API Packet
	{
	public:
		Packet(uint32_t packetType);
		virtual ~Packet();

		virtual bool ReadFromBuffer(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead) = 0;
		virtual bool WriteToBuffer(uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesWritten) const = 0;

		uint32_t GetPacketType() { return this->packetType; }

	private:
		uint32_t packetType;
	};

	/**
	 * 
	 */
	class CLUE_LIBRARY_API StringPacket : public Packet
	{
	public:
		StringPacket();
		virtual ~StringPacket();

		static uint32_t PacketType();

		virtual bool ReadFromBuffer(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead) override;
		virtual bool WriteToBuffer(uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesWritten) const override;

	public:
		std::string data;
	};

	/**
	 * 
	 */
	template<typename T>
	class StructurePacket : public Packet
	{
	public:
		StructurePacket() : Packet(PacketType())
		{
			::memset(&this->data, 0, sizeof(T));
		}

		virtual ~StructurePacket()
		{
		}

		static uint32_t PacketType() { return T::PacketType(); }

		virtual bool ReadFromBuffer(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead) override
		{
			if (sizeof(T) > bufferSize)
				return false;

			::memcpy(&this->data, buffer, sizeof(T));
			// TODO: What about byte-swapping?

			numBytesRead = sizeof(T);
			return true;
		}

		virtual bool WriteToBuffer(uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesWritten) const override
		{
			if (sizeof(T) > bufferSize)
				return false;

			::memcpy(buffer, &this->data, sizeof(T));
			numBytesWritten = sizeof(T);
			return true;
		}

	public:
		T data;
	};

	struct CharacterAndCards
	{
		static uint32_t PacketType();

		Character character;
		Card cardArray[CLUE_NUM_CHARACTERS + CLUE_NUM_ROOMS + CLUE_NUM_WEAPONS];
		uint32_t numCards;
	};

	struct PlayerIntroduction
	{
		static uint32_t PacketType();

		Character character;
		uint32_t numCards;
	};

	struct DiceRoll
	{
		static uint32_t PacketType();

		uint32_t rollAmount;
	};

	struct PlayerTravelRequested
	{
		static uint32_t PacketType();

		int nodeId;
	};

	struct PlayerTravelRejected
	{
		static uint32_t PacketType();

		enum Type : uint32_t
		{
			TARGET_NODE_DOESNT_EXIST,
			TARGET_NODE_TOO_FAR,
			TARGET_NODE_ALREADY_OCCUPIED
		};

		Type type;
	};

	struct PlayerTravelAccepted
	{
		static uint32_t PacketType();

		Character character;
		int nodeId;
	};

	struct PlayerCanMakeAccusation
	{
		static uint32_t PacketType();

		Room room;
	};

	struct PlayerMakesAccusation
	{
		static uint32_t PacketType();

		enum Flag
		{
			PASS			= 0x00000001,
			FINAL			= 0x00000002
		};

		Accusation accusation;
		uint32_t flags;
	};

	struct PlayerAccusationRejected
	{
		static uint32_t PacketType();

		enum Type : uint32_t
		{
			INVALID_ROOM
		};

		Type type;
	};

	struct PlayerMustRefuteIfPossible
	{
		static uint32_t PacketType();

		Accusation accusation;
	};

	struct PlayerAccustaionRefute
	{
		static uint32_t PacketType();

		Card card;
		uint32_t cannotRefute;
	};

	struct PresentAccusation
	{
		static uint32_t PacketType();

		Character accuser;
		Accusation accusation;
	};

	struct PlayerRefuteRejected
	{
		static uint32_t PacketType();

		enum Type : uint32_t
		{
			CARD_NOT_OWNED,
			CAN_REFUTE_BUT_DIDNT
		};

		Type type;
	};

	struct AccusationRefuted
	{
		static uint32_t PacketType();

		Character refuter;
	};

	struct AccusationRefutedWithCard
	{
		static uint32_t PacketType();

		Character refuter;
		Card card;
	};

	struct AccusationCouldNotBeRefuted
	{
		static uint32_t PacketType();

		Character character;
	};

	struct FinalAccusationResult
	{
		static uint32_t PacketType();

		Character accuser;
		uint32_t isCorrect;
	};

	/**
	 * 
	 */
	class PacketClassBase
	{
	public:
		virtual std::shared_ptr<Packet> Create() = 0;
	};

	/**
	 * 
	 */
	template<typename T>
	class PacketClass : public PacketClassBase
	{
	public:
		virtual std::shared_ptr<Packet> Create() override
		{
			return std::make_shared<T>();
		}
	};
}