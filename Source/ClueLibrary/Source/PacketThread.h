#pragma once

#include "Thread.h"
#include "Packet.h"
#include "RingBuffer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <map>

namespace Clue
{
	/**
	 *
	 */
	class CLUE_LIBRARY_API PacketThread : public Thread
	{
	public:
		PacketThread(SOCKET connectedSocket);
		virtual ~PacketThread();

		virtual bool Join() override;

		bool SendPacket(const std::shared_ptr<Packet> packet);
		bool ReceivePacket(std::shared_ptr<Packet>& packet);

		void PumpPacketSending();

	protected:

		virtual void Run() override;

	private:

		struct Header
		{
			uint32_t magic;
			uint32_t version;
			uint32_t packetType;
		};

		bool ReadPacket(const uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesRead, std::shared_ptr<Packet>& packet);
		bool WritePacket(uint8_t* buffer, uint32_t bufferSize, uint32_t& numBytesWritten, const std::shared_ptr<Packet> packet);

		SOCKET connectedSocket;
		ThreadSafeQueue<std::shared_ptr<Packet>> packetQueue;
		std::map<uint32_t, std::shared_ptr<PacketClassBase>> packetClassMap;
		RingBuffer outgoingPacketBuffer;
	};
}