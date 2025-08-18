#include "Packet.h"

using namespace Clue;

//------------------------------------- Packet -------------------------------------

Packet::Packet()
{
}

/*virtual*/ Packet::~Packet()
{
}

/*static*/ std::shared_ptr<Packet> Packet::Read(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead)
{
	if(bufferSize < sizeof(Header))
		return std::shared_ptr<Packet>();

	Header header;
	::memcpy(&header, buffer, sizeof(Header));

	if (header.magic != CLUE_PACKET_MAGIC)
	{
		// TODO: Byte-swap.  Fail for now.
		return std::shared_ptr<Packet>();
	}

	if (header.version != CLUE_PACKET_VERSION)
		return std::shared_ptr<Packet>();

	std::shared_ptr<Packet> packet;

	switch (header.payloadType)
	{
	case CLUE_PACKET_TYPE_STRING_PACKET:
		packet = std::make_shared<StringPacket>();
		break;
	}

	if (packet.get())
	{
		const uint8_t* payloadBuffer = &buffer[sizeof(Header)];
		uint32_t payloadBufferSize = bufferSize - sizeof(Header);
		uint32_t numPayloadBytesRead = 0;
		if (!packet->ReadFromBuffer(payloadBuffer, payloadBufferSize, numPayloadBytesRead))
			packet.reset();

		numBytesRead = sizeof(Header) + numPayloadBytesRead;
	}

	return packet;
}

/*static*/ bool Packet::Write(uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesWritten)
{
	return false;
}

//------------------------------------- StringPacket -------------------------------------

StringPacket::StringPacket()
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