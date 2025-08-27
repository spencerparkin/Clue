#include "PacketThread.h"
#include "RingBuffer.h"
#include <vector>
#include <assert.h>

using namespace Clue;

PacketThread::PacketThread(SOCKET connectedSocket) : outgoingPacketBuffer(4 * 1024)
{
	this->connectedSocket = connectedSocket;

	this->RegisterPacketClass<StringPacket>();
	this->RegisterPacketClass<StructurePacket<CharacterAndCards>>();
	this->RegisterPacketClass<StructurePacket<PlayerIntroduction>>();
	this->RegisterPacketClass<StructurePacket<DiceRoll>>();
	this->RegisterPacketClass<StructurePacket<PlayerTravelRequested>>();
	this->RegisterPacketClass<StructurePacket<PlayerTravelRejected>>();
	this->RegisterPacketClass<StructurePacket<PlayerTravelAccepted>>();
}

/*virtual*/ PacketThread::~PacketThread()
{
}

bool PacketThread::SendPacket(const std::shared_ptr<Packet> packet)
{
	if (this->connectedSocket == INVALID_SOCKET)
		return false;

	std::vector<char> sendBuffer;
	sendBuffer.resize(4 * 1024);
	uint32_t numBytesWritten = 0;
	if (!this->WritePacket((uint8_t*)sendBuffer.data(), sendBuffer.size(), numBytesWritten, packet))
		return false;

	bool dataWritten = this->outgoingPacketBuffer.WriteBytes((const uint8_t*)sendBuffer.data(), numBytesWritten);
	assert(dataWritten);
	return true;
}

bool PacketThread::ReceivePacket(std::shared_ptr<Packet>& packet)
{
	return this->packetQueue.Remove(packet);
}

/*virtual*/ bool PacketThread::Join()
{
	if (this->connectedSocket != INVALID_SOCKET)
	{
		::closesocket(this->connectedSocket);
		this->connectedSocket = INVALID_SOCKET;
	}

	return Thread::Join();
}

void PacketThread::PumpPacketSending()
{
	// We want to drain as much as we can each pump so that,
	// in theory, the out-going packet creation rate would
	// never exceed the out-going packet send rate.  That is
	// unlikely to ever happen with this program, but I rather
	// be as correct here as I possibly can in my implementation.
	while (true)
	{
		uint32_t numBytesToSend = this->outgoingPacketBuffer.GetNumStoredBytes();
		if (numBytesToSend == 0)
			break;

		std::vector<char> sendBuffer;
		sendBuffer.resize(numBytesToSend);
		this->outgoingPacketBuffer.PeakBytes((uint8_t*)sendBuffer.data(), numBytesToSend);

		uint32_t numBytesSent = ::send(this->connectedSocket, sendBuffer.data(), numBytesToSend, 0);
		if (numBytesSent == 0)
			break;
		
		this->outgoingPacketBuffer.DeleteBytes(numBytesSent);
	}
}

/*virtual*/ void PacketThread::Run()
{
	RingBuffer ringBuffer(16 * 1024);
	std::vector<char> recvBuffer;
	recvBuffer.resize(4 * 1024);

	while (true)
	{
		// Block here until we read some data from the socket.  Note that closing the socket from the main thread will signal us to exit.
		uint32_t numBytesReceived = (uint32_t)::recv(this->connectedSocket, recvBuffer.data(), int(recvBuffer.size()), 0);
		if (numBytesReceived == SOCKET_ERROR)
			break;

		if (!ringBuffer.WriteBytes((const uint8_t*)recvBuffer.data(), numBytesReceived))
			break;

		// It's really important to drain the buffer as much as possible before we continue.
		// We don't want to be blocked on the socket with packets pending in the buffer.
		while (true)
		{
			uint32_t numStoredBytes = ringBuffer.GetNumStoredBytes();
			std::unique_ptr<uint8_t> buffer(new uint8_t[numStoredBytes]);
			if (!ringBuffer.PeakBytes(buffer.get(), numStoredBytes))
				break;

			uint32_t numBytesRead = 0;
			std::shared_ptr<Packet> packet;
			if (!this->ReadPacket(buffer.get(), numStoredBytes, numBytesRead, packet))
				break;

			if (!ringBuffer.DeleteBytes(numBytesRead))
				break;

			this->packetQueue.Add(packet);
		}
	}
}

bool PacketThread::ReadPacket(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead, std::shared_ptr<Packet>& packet)
{
	if (bufferSize < sizeof(Header))
		return false;

	Header header;
	::memcpy(&header, buffer, sizeof(Header));

	if (header.magic != CLUE_PACKET_MAGIC)
	{
		// TODO: Byte-swap.  Fail for now.
		return false;
	}

	if (header.version != CLUE_PACKET_VERSION)
		return false;

	std::map<uint32_t, std::shared_ptr<PacketClassBase>>::iterator iter = this->packetClassMap.find(header.packetType);
	if (iter == this->packetClassMap.end())
		return false;

	std::shared_ptr<PacketClassBase> packetClass = iter->second;
	packet = packetClass->Create();

	const uint8_t* payloadBuffer = &buffer[sizeof(Header)];
	uint32_t payloadBufferSize = bufferSize - sizeof(Header);
	uint32_t numPayloadBytesRead = 0;
	if (!packet->ReadFromBuffer(payloadBuffer, payloadBufferSize, numPayloadBytesRead))
	{
		packet.reset();
		return false;
	}

	numBytesRead = sizeof(Header) + numPayloadBytesRead;
	return true;
}

bool PacketThread::WritePacket(uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesWritten, const std::shared_ptr<Packet> packet)
{
	if (bufferSize < sizeof(Header))
		return false;

	auto header = reinterpret_cast<Header*>(buffer);
	header->magic = CLUE_PACKET_MAGIC;
	header->version = CLUE_PACKET_VERSION;
	header->packetType = packet->GetPacketType();

	uint8_t* payloadBuffer = &buffer[sizeof(Header)];
	uint32_t payloadBufferSize = bufferSize - sizeof(Header);
	uint32_t numPayloadBytesWritten = 0;
	if (!packet->WriteToBuffer(payloadBuffer, payloadBufferSize, numPayloadBytesWritten))
		return false;

	numBytesWritten = sizeof(Header) + numPayloadBytesWritten;
	return true;
}