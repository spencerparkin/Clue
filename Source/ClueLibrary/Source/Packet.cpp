#include "Packet.h"

using namespace Clue;

//------------------------------------- Packet -------------------------------------

Packet::Packet(uint32_t packetType)
{
	this->packetType = packetType;
}

/*virtual*/ Packet::~Packet()
{
}

//------------------------------------- StringPacket -------------------------------------

StringPacket::StringPacket(uint32_t packetType /*= CLUE_PACKET_TYPE_STRING_PACKET*/) : Packet(packetType)
{
}

/*virtual*/ StringPacket::~StringPacket()
{
}

/*virtual*/ bool StringPacket::ReadFromBuffer(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead)
{
	if (bufferSize == 0)
		return false;

	uint32_t i = 0;
	while (buffer[i] != '\0')
		if (++i == bufferSize)
			return false;

	this->data = std::string((const char*)buffer);
	numBytesRead = i;
	return true;
}

/*virtual*/ bool StringPacket::WriteToBuffer(uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesWritten) const
{
	uint32_t length = this->data.length();
	if (length + 1 > bufferSize)
		return false;

	::memcpy(buffer, this->data.c_str(), length);
	buffer[length] = '\0';
	numBytesWritten = length + 1;
	return true;
}