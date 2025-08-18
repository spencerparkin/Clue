#include "PacketThread.h"
#include "RingBuffer.h"
#include <vector>

using namespace Clue;

PacketThread::PacketThread(SOCKET connectedSocket)
{
	this->connectedSocket = connectedSocket;

	this->packetClassMap.insert(std::pair<uint32_t, std::shared_ptr<PacketClassBase>>(uint32_t(CLUE_PACKET_TYPE_STRING_PACKET), std::make_shared<PacketClass<StringPacket>>()));
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

	// TODO: Loop here until all bytes sent or is that bad?
	int numBytesSent = ::send(this->connectedSocket, sendBuffer.data(), numBytesWritten, 0);
	return numBytesSent == numBytesWritten;
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
	packet = packetClass->Create(header.packetType);

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

	Header header;
	header.magic = CLUE_PACKET_MAGIC;
	header.version = CLUE_PACKET_VERSION;
	header.packetType = packet->packetType;

	uint8_t* payloadBuffer = &buffer[sizeof(Header)];
	uint32_t payloadBufferSize = bufferSize - sizeof(Header);
	uint32_t numPayloadBytesWritten = 0;
	if (!packet->WriteToBuffer(payloadBuffer, payloadBufferSize, numPayloadBytesWritten))
		return false;

	numBytesWritten = sizeof(Header) + numPayloadBytesWritten;
	return true;
}