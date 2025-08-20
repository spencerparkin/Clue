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
#define CLUE_PACKET_TYPE_STRING						0x00000001
#define CLUE_PACKET_TYPE_CHAR_AND_CARDS				0x00000002
#define CLUE_PACKET_TYPE_PLAYER_INTRO				0x00000003
#define CLUE_PACKET_TYPE_DICE_ROLL					0x00000004
#define CLUE_PACKET_TYPE_PLAYER_TRAVEL_REQUESTED	0x00000005
#define CLUE_PACKET_TYPE_PLAYER_TRAVEL_REJECTED		0x00000006
#define CLUE_PACKET_TYPE_PLAYER_TRAVEL_ACCEPTED		0x00000007

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

		uint32_t packetType;
	};

	/**
	 * 
	 */
	class CLUE_LIBRARY_API StringPacket : public Packet
	{
	public:
		StringPacket(uint32_t packetType = CLUE_PACKET_TYPE_STRING);
		virtual ~StringPacket();

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
		StructurePacket(uint32_t packetType) : Packet(packetType)
		{
			::memset(&this->data, 0, sizeof(T));
		}

		virtual ~StructurePacket()
		{
		}

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
		Character character;
		Card cardArray[CLUE_NUM_CHARACTERS + CLUE_NUM_ROOMS + CLUE_NUM_WEAPONS];
		uint32_t numCards;
		uint32_t id;
	};

	struct PlayerIntroduction
	{
		Character character;
		uint32_t numCards;
	};

	struct DiceRoll
	{
		uint32_t rollAmount;
	};

	struct PlayerTravelRequested
	{
		int nodeId;
	};

	struct PlayerTravelRejected
	{
		enum Type : uint32_t
		{
			TARGET_NODE_DOESNT_EXIST,
			TARGET_NODE_TOO_FAR
		};

		Type type;
	};

	struct PlayerTravelAccepted
	{
		Character character;
		int nodeId;
	};

	/**
	 * 
	 */
	class PacketClassBase
	{
	public:
		virtual std::shared_ptr<Packet> Create(uint32_t packetType) = 0;

		uint32_t payloadType;
	};

	/**
	 * 
	 */
	template<typename T>
	class PacketClass : public PacketClassBase
	{
	public:
		virtual std::shared_ptr<Packet> Create(uint32_t packetType) override
		{
			return std::make_shared<T>(packetType);
		}
	};
}