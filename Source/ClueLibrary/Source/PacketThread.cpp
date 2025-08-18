#include "PacketThread.h"
#include "RingBuffer.h"
#include <vector>

using namespace Clue;

PacketThread::PacketThread(SOCKET connectedSocket)
{
	this->connectedSocket = connectedSocket;
}

/*virtual*/ PacketThread::~PacketThread()
{
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
			std::shared_ptr<Packet> packet = Packet::Read(buffer.get(), numStoredBytes, numBytesRead);
			if (!packet.get())
				break;

			if (!ringBuffer.DeleteBytes(numBytesRead))
				break;

			this->packetQueue.Add(packet);
		}
	}
}