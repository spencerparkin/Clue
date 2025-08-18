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
		Packet();
		virtual ~Packet();

		virtual bool ReadFromBuffer(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead) = 0;
		virtual bool WriteToBuffer(uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesWritten) const = 0;

		static std::shared_ptr<Packet> Read(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead);
		static bool Write(uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesWritten);

		struct Header
		{
			uint32_t magic;
			uint32_t version;
			uint32_t payloadType;
		};
	};

	/**
	 * 
	 */
	class CLUE_LIBRARY_API StringPacket : public Packet
	{
	public:
		StringPacket();
		virtual ~StringPacket();

		virtual bool ReadFromBuffer(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead) override;
		virtual bool WriteToBuffer(uint8_t* buffer, uint32_t bufferSize, uint32_t& numByteWritten) const override;

		std::string data;
	};
}