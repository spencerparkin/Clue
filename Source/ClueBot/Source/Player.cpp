#include "Player.h"

using namespace Clue;

Player::Player(const std::string& ipAddr, int port)
{
	this->ipAddr = ipAddr;
	this->port = port;
	this->keepRunning = true;
}

/*virtual*/ Player::~Player()
{
}

/*virtual*/ bool Player::Join()
{
	this->keepRunning = false;

	return Thread::Join();
}

/*virtual*/ void Player::Run()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA startupData;
	if (WSAStartup(version, &startupData) != 0)
		return;

	addrinfo hints;
	::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	char portStr[32];
	sprintf_s(portStr, sizeof(portStr), "%d", this->port);
	addrinfo* addressInfo = nullptr;
	int result = ::getaddrinfo(this->ipAddr.c_str(), portStr, &hints, &addressInfo);
	if (result != 0)
		return;

	if (addressInfo->ai_protocol != IPPROTO_TCP)
		return;

	SOCKET connectedSocket = ::socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol);
	if (connectedSocket == INVALID_SOCKET)
		return;

	bool connected = false;
	int numAttemps = 0;
	int maxAttemps = 10;
	while (this->keepRunning)
	{
		bool abort = false;
		printf("Trying to connect to %s on port %d...\n", this->ipAddr.c_str(), this->port);

		int result = ::connect(connectedSocket, addressInfo->ai_addr, (int)addressInfo->ai_addrlen);
		if (result != SOCKET_ERROR)
		{
			printf("Connected!\n");
			connected = true;
			break;
		}

		if (++numAttemps > maxAttemps)
		{
			printf("Failed to connect!\n");
			return;
		}

		::Sleep(100);
	}

	freeaddrinfo(addressInfo);

	this->packetThread = std::make_shared<PacketThread>(connectedSocket);
	if (!this->packetThread->Split())
		return;

	while (this->keepRunning)
	{
		// We could be faster here if we blocked on a semaphore who's count reflected
		// the size of the packet queue, but this is fine for now.
		std::shared_ptr<Packet> packet;
		if (!this->packetThread->ReceivePacket(packet))
			::Sleep(100);

		if (!this->ProcessPacket(packet))
			break;
	}

	this->packetThread->Join();

	::WSACleanup();
}

/*virtual*/ bool Player::ProcessPacket(std::shared_ptr<Packet> packet)
{
	return true;
}