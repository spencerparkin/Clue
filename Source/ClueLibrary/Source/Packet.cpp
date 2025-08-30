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

StringPacket::StringPacket() : Packet(PacketType())
{
}

/*virtual*/ StringPacket::~StringPacket()
{
}

/*static*/ uint32_t StringPacket::PacketType()
{
	return __COUNTER__;
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

//------------------------------------- CharacterAndCards -------------------------------------

/*static*/ uint32_t CharacterAndCards::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerIntroduction -------------------------------------

/*static*/ uint32_t PlayerIntroduction::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- DiceRoll -------------------------------------

/*static*/ uint32_t DiceRoll::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerTravelRequested -------------------------------------

/*static*/ uint32_t PlayerTravelRequested::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerTravelRejected -------------------------------------

/*static*/ uint32_t PlayerTravelRejected::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerTravelAccepted -------------------------------------

/*static*/ uint32_t PlayerTravelAccepted::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerCanMakeAccusation -------------------------------------

/*static*/ uint32_t PlayerCanMakeAccusation::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerMakesAccusation -------------------------------------

/*static*/ uint32_t PlayerMakesAccusation::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerAccusationRejected -------------------------------------

/*static*/ uint32_t PlayerAccusationRejected::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerMustRefuteIfPossible -------------------------------------

/*static*/ uint32_t PlayerMustRefuteIfPossible::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerAccustaionRefute -------------------------------------

/*static*/ uint32_t PlayerAccustaionRefute::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PresentAccusation -------------------------------------

/*static*/ uint32_t PresentAccusation::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- PlayerRefuteRejected -------------------------------------

/*static*/ uint32_t PlayerRefuteRejected::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- AccusationRefuted -------------------------------------

/*static*/ uint32_t AccusationRefuted::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- AccusationRefutedWithCard -------------------------------------

/*static*/ uint32_t AccusationRefutedWithCard::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- AccusationCouldNotBeRefuted -------------------------------------

/*static*/ uint32_t AccusationCouldNotBeRefuted::PacketType()
{
	return __COUNTER__;
}

//------------------------------------- FinalAccusationResult -------------------------------------

/*static*/ uint32_t FinalAccusationResult::PacketType()
{
	return __COUNTER__;
}