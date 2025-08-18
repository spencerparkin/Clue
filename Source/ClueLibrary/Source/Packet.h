#pragma once

#include "Defines.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdint.h>
#include <memory>
#include <string>

#define CLUE_PACKET_VERSION					0x00000001
#define CLUE_PACKET_MAGIC					0xDEADBEEF
#define CLUE_PACKET_TYPE_STRING_PACKET		0x00000001

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
		StringPacket(uint32_t packetType = CLUE_PACKET_TYPE_STRING_PACKET);
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