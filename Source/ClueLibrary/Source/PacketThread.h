#pragma once

#include "Thread.h"
#include "Packet.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

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

	protected:

		virtual void Run() override;

	private:
		SOCKET connectedSocket;
		ThreadSafeQueue<std::shared_ptr<Packet>> packetQueue;
	};
}