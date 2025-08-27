#include "Player.h"
#include "PacketHandler.h"

using namespace Clue;

Player::Player(const std::string& ipAddr, int port)
{
	this->ipAddr = ipAddr;
	this->port = port;
	this->keepRunning = true;
	this->packetHandlerMap.insert(std::pair(CharacterAndCards::PacketType(), std::make_shared<CharAndCardsPacketHandler>()));
	this->packetHandlerMap.insert(std::pair(PlayerIntroduction::PacketType(), std::make_shared<PlayerIntroPacketHandler>()));
	this->packetHandlerMap.insert(std::pair(DiceRoll::PacketType(), std::make_shared<DiceRollPacketHandler>()));
	this->packetHandlerMap.insert(std::pair(PlayerTravelAccepted::PacketType(), std::make_shared<TravelAcceptedHandler>()));
	this->gameData.boardGraph.Regenerate();
}

/*virtual*/ Player::~Player()
{
}

Player::GameData* Player::GetGameData()
{
	return &this->gameData;
}

Clue::PacketThread* Player::GetPacketThread()
{
	return this->packetThread.get();
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

	while (this->keepRunning && this->packetThread->IsRunning())
	{
		// We could be faster here if we blocked on a semaphore who's count reflected
		// the size of the packet queue, but this is fine for now.
		std::shared_ptr<Packet> packet;
		if (!this->packetThread->ReceivePacket(packet))
			::Sleep(500);
		else if (!this->ProcessPacket(packet))
			break;

		this->packetThread->PumpPacketSending();
	}

	this->packetThread->Join();

	::WSACleanup();
}

/*virtual*/ bool Player::ProcessPacket(const std::shared_ptr<Packet> packet)
{
	std::map<uint32_t, std::shared_ptr<PacketHandler>>::iterator iter = this->packetHandlerMap.find(packet->GetPacketType());
	if (iter == this->packetHandlerMap.end())
		return false;

	std::shared_ptr<PacketHandler> packetHandler = iter->second;
	if (!packetHandler->HandlePacket(packet, this))
		return false;

	return true;
}